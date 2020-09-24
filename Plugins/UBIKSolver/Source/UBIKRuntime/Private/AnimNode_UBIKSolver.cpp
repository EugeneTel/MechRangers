// 2020 Sticky Snout Studio (Jonas Molgaard)

#include "AnimNode_UBIKSolver.h"
#include "UBIKRuntime.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

/** STATS FOR USE WITH PROFILER **/
DECLARE_CYCLE_STAT(TEXT("UBIK_EvaluateThread"), STAT_UBIK_EvaluateThread, STATGROUP_Character);

void FAnimNode_UBIKSolver::GatherDebugData(FNodeDebugData& DebugData)
{
	FAnimNode_SkeletalControlBase::GatherDebugData(DebugData);
}

void FAnimNode_UBIKSolver::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);

	Context.AnimInstanceProxy->GetSkelMeshComponent()->SetTickGroup(TG_DuringPhysics);
}

void FAnimNode_UBIKSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	// Optimally i would like to make this check in IsValidToEvaluate(), but that doesn't seem to work.
	if (Output.AnimInstanceProxy->GetSkelMeshComponent() && Output.AnimInstanceProxy->GetSkelMeshComponent()->GetWorld())
	{
		World = Output.AnimInstanceProxy->GetSkelMeshComponent()->GetWorld();
		if (!(World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE))
		{
			//GEngine->AddOnScreenDebugMessage((uint64)-1, 2.f, FColor::Cyan, TEXT("Game is not running"));
			//UE_LOG(LogUBIKRuntime, Display, TEXT("Game is running"));
			return;
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage((uint64)-1, 2.f, FColor::Cyan, TEXT("Game is not running"));
		//UE_LOG(LogUBIKRuntime, Display, TEXT("Game is running"));
		return;
	}

	SCOPE_CYCLE_COUNTER(STAT_UBIK_EvaluateThread);

	check(OutBoneTransforms.Num() == 0);

	//UE_LOG(LogUBIKRuntime, Display, TEXT("DeltaTime: %f"), DeltaTime);
	MeshComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();
	ComponentSpaceW = Output.AnimInstanceProxy->GetComponentTransform();
	ComponentSpace = ComponentSpaceW.Inverse();

	ConvertTransforms();
	
	SetShoulder();
	SetLeftUpperArm();
	SetRightUpperArm();

	ResetUpperArmsLocation();
	SolveArms();
	
	BaseCharTransformC = GetBaseCharTransform();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("BaseCharTransformC loc: %s"), *BaseCharTransformC.GetTranslation().ToString());

	// TODO: Disabled for now, because it crashes engine for some reason.
	//if (bDrawDebug)
	//{
	//	DrawDebug();
	//}
	
	// Stuff from the AnimBP
	Head = UKismetMathLibrary::ComposeRotators(FRotator(90.f, 0.f, 90.f), HeadTransformC.Rotator());
	Spine03 = FMath::RInterpTo(Spine03, BaseCharTransformC.Rotator(), DeltaTime, Settings.BodyInterSpeed);
	Spine02 = FMath::RInterpTo(Spine02, Spine03, DeltaTime, Settings.BodyInterSpeed);
	Spine01 = FMath::RInterpTo(Spine01, Spine02, DeltaTime, Settings.BodyInterSpeed);
	Pelvis = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(90.f, 0.f, 90.f), Spine01), BaseCharTransformC.GetTranslation(), FVector::OneVector);

	Clavicle_l = LeftClavicleC.Rotator();
	UpperArm_l = LeftUpperArmTransformC.Rotator();
	LowerArm_l = LeftLowerArmTransformC.Rotator();
	Hand_l = FRotator(LeftHandTransformC.Rotator().Quaternion() * FRotator(0.f, -25.f, 180.f).Quaternion());

	Clavicle_r = FRotator(RightClavicleC.Rotator().Quaternion() * FRotator(180.f, 0.f, 0.f).Quaternion());
	UpperArm_r = FRotator(RightUpperArmTransformC.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("UpperArm_r: %s"), *UpperArm_r.ToString());
	LowerArm_r = FRotator(RightLowerArmTransformC.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
	Hand_r = FRotator(RightHandTransformC.Rotator().Quaternion() * FRotator(180.f, 25.f, 180.f).Quaternion());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Clavicle_r: %s"), *Clavicle_r.ToString());

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	SetBoneTransform(PelvisBoneToModify, Pelvis, Output, BoneContainer, true, !bIgnoreLocation);

	SetBoneTransform(Spine01_BoneToModify, FTransform(UKismetMathLibrary::ComposeRotators(FRotator(83.f, 0.f, 90.f), Spine01)), Output, BoneContainer, true);
	SetBoneTransform(Spine02_BoneToModify, FTransform(UKismetMathLibrary::ComposeRotators(FRotator(104.0f, 0.f, 90.f), Spine02)), Output, BoneContainer, true);
	SetBoneTransform(Spine03_BoneToModify, FTransform(UKismetMathLibrary::ComposeRotators(FRotator(86.22f, 0.f, 90.f), Spine03)), Output, BoneContainer, true);
	SetBoneTransform(HeadBoneToModify, FTransform(Head), Output, BoneContainer, true);
	
	SetBoneTransform(LeftClavicleBoneToModify, FTransform(Clavicle_l), Output, BoneContainer, true);
	SetBoneTransform(LeftUpperArmBoneToModify, FTransform(UpperArm_l), Output, BoneContainer, true);
	SetBoneTransform(LeftLowerArmBoneToModify, FTransform(LowerArm_l), Output, BoneContainer, true);
	SetBoneTransform(LeftHandBoneToModify, FTransform(Hand_l), Output, BoneContainer, true);

	SetBoneTransform(RightClavicleBoneToModify, FTransform(Clavicle_r), Output, BoneContainer, true);
	SetBoneTransform(RightUpperArmBoneToModify, FTransform(UpperArm_r), Output, BoneContainer, true);
	SetBoneTransform(RightLowerArmBoneToModify, FTransform(LowerArm_r), Output, BoneContainer, true);
	SetBoneTransform(RightHandBoneToModify, FTransform(Hand_r), Output, BoneContainer, true);
}

void FAnimNode_UBIKSolver::SetBoneTransform(const FBoneReference& BoneToModify, FTransform Transform, FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, bool bApplyRotation, bool bApplyTranslation)
{
	TArray<FBoneTransform> TempTransform;

	FCompactPoseBoneIndex CompactBoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);
	FTransform NewTransform = Output.Pose.GetComponentSpaceTransform(CompactBoneIndex);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("GetBoneTransform, Index: %i Name: %s"), CompactBoneIndex.GetInt(), *BoneToModify.BoneName.ToString());

	if (CompactBoneIndex != INDEX_NONE)
	{
		if (bApplyRotation)
		{
			NewTransform.SetRotation(Transform.GetRotation());
		}

		if (bApplyTranslation)
		{
			NewTransform.SetTranslation(Transform.GetTranslation());
		}        
		TempTransform.Add(FBoneTransform(CompactBoneIndex, NewTransform));
		Output.Pose.LocalBlendCSBoneTransforms(TempTransform, Alpha);
	}
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return true;
}

void FAnimNode_UBIKSolver::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	DeltaTime = Context.GetDeltaTime();
}

void FAnimNode_UBIKSolver::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	HeadBoneToModify.Initialize(RequiredBones);
	LeftClavicleBoneToModify.Initialize(RequiredBones);
	RightClavicleBoneToModify.Initialize(RequiredBones);
	LeftUpperArmBoneToModify.Initialize(RequiredBones);
	RightUpperArmBoneToModify.Initialize(RequiredBones);
	LeftLowerArmBoneToModify.Initialize(RequiredBones);
	RightLowerArmBoneToModify.Initialize(RequiredBones);
	LeftHandBoneToModify.Initialize(RequiredBones);
	RightHandBoneToModify.Initialize(RequiredBones);
	Spine01_BoneToModify.Initialize(RequiredBones);
	Spine02_BoneToModify.Initialize(RequiredBones);
	Spine03_BoneToModify.Initialize(RequiredBones);
	PelvisBoneToModify.Initialize(RequiredBones);
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, ShoulderTransform, LeftTransformW, RightTransformW, 
* HeadTransformC, LeftTransformC, RightTransformC
**/
void FAnimNode_UBIKSolver::ConvertTransforms()
{
	// Technically ShoulderTransformC should be set 1st
	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	ShoulderTransform = ShoulderTransformW.Inverse();

	//UE_LOG(LogUBIKRuntime, Display, TEXT("1:LeftHandTransformW: %s"), *LeftHandTransformW.GetTranslation().ToString());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Settings.LocalHandOffset: %s"), *Settings.LocalHandOffset.GetTranslation().ToString());
	LeftHandTransformW = UUBIK::AddLocalOffset(LeftHandTransformW, Settings.LocalHandOffset);

	//DebugDrawAxes(LeftHandTransformW, true);

	//UE_LOG(LogUBIKRuntime, Display, TEXT("2:LeftHandTransformW: %s"), *LeftHandTransformW.GetTranslation().ToString());
	// Mirror Y-direction otherwise it will move opposite ways.
	RightHandTransformW = UUBIK::AddLocalOffset(RightHandTransformW, Settings.LocalHandOffset * FVector(1.f, -1.f, 1.f));

	HeadTransformC = HeadTransformW * ComponentSpace;
	LeftHandTransformC = LeftHandTransformW * ComponentSpace;
	RightHandTransformC = RightHandTransformW * ComponentSpace;
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, HeadTransformS, LeftTransformS, RightTransformS
**/
void FAnimNode_UBIKSolver::SetShoulder()
{
	FRotator RotationFromHead = GetShoulderRotationFromHead();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RotationFromHead: %s"), *RotationFromHead.ToString());

	FRotator RotationFromHands = GetShoulderRotationFromHands();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RotationFromHands: %s"), *RotationFromHands.ToString());

	FRotator Rotation = FRotator(0.f, UKismetMathLibrary::RLerp(RotationFromHead, RotationFromHands, Settings.ShoulderHeadHandAlpha, true).Yaw, 0.f);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Rotation: %s"), *Rotation.ToString());

	FVector Translation = GetShoulderLocation();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("GetShoulderLocation: %s"), *Translation.ToString());

	ShoulderTransformC = FTransform(Rotation, Translation, FVector::OneVector);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("ShoulderTransformC: %s"), *ShoulderTransformC.Rotator().ToString());

	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	HeadTransformS = HeadTransformW * ShoulderTransform;
	LeftHandTransformS = LeftHandTransformW * ShoulderTransform;
	RightHandTransformS = RightHandTransformW * ShoulderTransform;
}

FVector FAnimNode_UBIKSolver::GetShoulderLocation()
{
	FVector Offset = FVector(-9.f, 0.f, -7.f) + FVector(Settings.BaseCharOffset.X, Settings.BaseCharOffset.Y, 0.f);
	FRotator HeadRotator = HeadTransformC.Rotator();
	FRotator HeadYaw = FRotator(0.f, HeadRotator.Yaw, HeadRotator.Roll);

	Offset = HeadYaw.RotateVector(Offset);

	FRotator Delta = FRotator(HeadRotator.Pitch, 0.f, 0.f);

	FVector ShoulderTranslation = HeadTransformC.GetTranslation() + Offset;
	FTransform TempShoulder = FTransform(FRotator::ZeroRotator, ShoulderTranslation, FVector::OneVector);
	
	FTransform TempRotatedShoulder = UUBIK::RotatePointAroundPivot(TempShoulder, HeadTransformC, Delta);
	
	return TempRotatedShoulder.GetTranslation() + FVector(0.f, 0.f, -17.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHead()
{
	return FRotator(0.f, HeadTransformC.Rotator().Yaw, 0.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHands()
{
	FVector Offset = FVector(0.f, 0.f, 15.f);
	FVector TopHeadTranslation = HeadTransformW.GetTranslation() + HeadTransformW.Rotator().RotateVector(Offset);

	FVector Hand = (LeftHandTransformW * HeadTransformW.Inverse()).GetTranslation();
	FVector HandHeadDelta = LeftHandTransformW.GetTranslation() - TopHeadTranslation;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LEFTHAND: LeftTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *LeftHandTransformW.ToString(), LeftHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());

	LeftHeadHandAngle = GetHeadHandAngle(LeftHeadHandAngle, Hand, HandHeadDelta);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftHeadHandAngle: %f"), LeftHeadHandAngle);

	Hand = (RightHandTransformW * HeadTransformW.Inverse()).GetTranslation();
	HandHeadDelta = RightHandTransformW.GetTranslation() - TopHeadTranslation;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RIGHTHAND: RightTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *RightTransformW.ToString(), RightHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());
	RightHeadHandAngle = GetHeadHandAngle(RightHeadHandAngle, Hand, HandHeadDelta);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RightHeadHandAngle: %f"), RightHeadHandAngle);

	FRotator YawRotator = FRotator(0.f, ((LeftHeadHandAngle + RightHeadHandAngle) / 2.f), 0.f);
	FTransform TempTransform = FTransform(YawRotator, FVector::ZeroVector, FVector::OneVector);

	// Back in WorldSpace
	TempTransform = TempTransform * HeadTransformW;

	TempTransform = TempTransform * ComponentSpace;

	return TempTransform.Rotator();
}

float FAnimNode_UBIKSolver::GetHeadHandAngle(float LastAngle, FVector Hand, FVector HandHeadDelta)
{
	float HeadHandAlpha = UKismetMathLibrary::MapRangeClamped(FVector(HandHeadDelta.X, HandHeadDelta.Y, 0.f).Size(), 20.f, 50.f, 0.f, 1.f);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("HeadHandAlpha: %f"), HeadHandAlpha);

	FVector Hand2D = FVector(Hand.X, Hand.Y, 0.f);
	FVector Hand2DNormalized = Hand2D.GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Hand2DNormalized.Y, Hand2DNormalized.X));
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Angle: %f"), Angle);

	bool Selector1 = FVector::DotProduct(Hand2DNormalized, FVector::ForwardVector) > Settings.HeadHandAngleLimitDot;
	bool Selector2 = (FMath::Sign(LastAngle) == FMath::Sign(Angle)) || 
		(Angle < Settings.OkSpanAngle && Angle > -1.0f * Settings.OkSpanAngle);
	bool Selector = Selector1 && Selector2;

	float SelectedFloat = (Selector) ? (Angle) : (Settings.HeadHandAngleLimit * FMath::Sign(LastAngle));
	//UE_LOG(LogUBIKRuntime, Display, TEXT("SelectedFloat: %f"), SelectedFloat);

	return FMath::Lerp(0.f, SelectedFloat, HeadHandAlpha);
}

void FAnimNode_UBIKSolver::SetLeftUpperArm()
{
	LeftUpperArmTransformS = RotateUpperArm(true, LeftHandTransformS.GetTranslation());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftUpperArmTransformS %s"), *LeftUpperArmTransformS.ToString());

	FVector X = (LeftUpperArmTransformS * ShoulderTransformW).GetTranslation() - ShoulderTransformW.GetTranslation();
	FVector Z = FVector::UpVector;

	LeftClavicleC = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) * ComponentSpace;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftClavicleC %s"), *LeftClavicleC.ToString());
}

void FAnimNode_UBIKSolver::SetRightUpperArm()
{
	RightUpperArmTransformS = RotateUpperArm(false, RightHandTransformS.GetTranslation());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RightUpperArmTransformS %s"), *RightUpperArmTransformS.ToString());

	FVector X = (RightUpperArmTransformS * ShoulderTransformW).GetTranslation() - ShoulderTransformW.GetTranslation();
	FVector Z = FVector::UpVector;

	RightClavicleC = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) * ComponentSpace;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RightClavicleC %s"), *RightClavicleC.ToString());
}

FTransform FAnimNode_UBIKSolver::RotateUpperArm(bool IsLeftArm, FVector HandTranslation)
{
	float Sign = (IsLeftArm) ? (1.f) : (-1.f);

	FVector InitialUpperArmTranslation = FVector::RightVector * Settings.UpperArmsDistance / (2.f * Sign);
	FVector HandUpperArmDirection = HandTranslation - InitialUpperArmTranslation;
	
	float ForwardDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::ForwardVector) / Settings.ArmLength;
	float UpwardsDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::UpVector) / Settings.ArmLength;
	
	float Yaw;
	if (ForwardDistanceRatio > 0)
	{
		float TempYaw = (ForwardDistanceRatio - 0.5f) * Settings.DistinctShoulderRotationMultiplier;
		Yaw = FMath::Clamp(TempYaw, 0.f, Settings.DistinctShoulderRotationLimit) + Settings.ClavicleOffset;
	}
	else
	{
		float TempYaw = (ForwardDistanceRatio - 0.08f) * Settings.DistinctShoulderRotationMultiplier;
		Yaw = FMath::Clamp(TempYaw, -Settings.DistinctShoulderRotationLimit, 0.f) + Settings.ClavicleOffset;
	}

	float TempRoll = (UpwardsDistanceRatio - 0.2f)* Settings.DistinctShoulderRotationMultiplier;
	float Roll = FMath::Clamp(TempRoll, 0.f, Settings.DistinctShoulderRotationLimit);

	float NotSign = (IsLeftArm) ? (-1.f) : (1.f);
	FRotator Rotation = FRotator(0.f, Yaw * NotSign, Roll * NotSign);

	return FTransform(Rotation, InitialUpperArmTranslation, FVector::OneVector).Inverse();
}

/** The purpose of this function, is to make sure the UpperArm_l and UpperArm_r locations 
* are used in the coming calculations to ensure they reach right angle.
* They do that by setting LeftUpperArmTransformS and RightUpperArmTransformS.
**/
void FAnimNode_UBIKSolver::ResetUpperArmsLocation()
{
	if (MeshComponent && MeshComponent->SkeletalMesh)
	{
		FVector LeftUpperArm = MeshComponent->GetSocketTransform(LeftUpperArmBoneToModify.BoneName, RTS_World).GetTranslation();
		LeftUpperArmTransformS = FTransform(UKismetMathLibrary::InverseTransformLocation(ShoulderTransformW, LeftUpperArm));
		//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftUpperArm %s"), *LeftUpperArm.ToString());

		FVector RightUpperArm = MeshComponent->GetSocketTransform(RightUpperArmBoneToModify.BoneName, RTS_World).GetTranslation();
		RightUpperArmTransformS = FTransform(UKismetMathLibrary::InverseTransformLocation(ShoulderTransformW, RightUpperArm));
	}
}

void FAnimNode_UBIKSolver::SolveArms()
{
	/** LEFT ARM **/
	FVector HandLoc = LeftHandTransformS.GetTranslation();
	FRotator HandRot = LeftHandTransformS.GetRotation().Rotator();
	FVector UpperArmLoc = LeftUpperArmTransformS.GetTranslation();

	FTransform UpperArmBase = FTransform::Identity;
	FTransform LowerArmBase = FTransform::Identity;
	FTransform UpperArm = FTransform::Identity;
	FTransform LowerArm = FTransform::Identity;

	//UE_LOG(LogUBIKRuntime, Display, TEXT("UA: %s H: %s"), *UpperArmLoc.ToString(), *HandLoc.ToString()); 
	SetElbowBasePosition(UpperArmLoc, HandLoc, true, UpperArmBase, LowerArmBase);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArmBase: %s"), *LowerArmBase.ToString());

	float BaseAngle = RotateElbowByHandPosition(HandLoc, true);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("BaseAngle: %f"), BaseAngle);

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, UpperArm, LowerArm);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArm: %s"), *LowerArm.ToString());

	float Angle = RotateElbowByHandRotation(LowerArm, HandRot);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Angle: %f"), Angle);

	float SafeAngle = UUBIK::SafeguardAngle(LeftElbowHandAngle, Angle, 120.f);

	LeftElbowHandAngle = FMath::FInterpTo(LeftElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftElbowHandAngle: %f"), LeftElbowHandAngle);

	RotateElbow(LeftElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, LeftUpperArmTransformS, LeftLowerArmTransformS);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LeftElbowHandAngle + BaseAngle: %f"), LeftElbowHandAngle + BaseAngle);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LUAS: %s LLAS: %s"), *LeftUpperArmTransformS.ToString(), *LeftLowerArmTransformS.ToString());

	LeftUpperArmTransformW = LeftUpperArmTransformS * ShoulderTransformW;
	LeftLowerArmTransformW = LeftLowerArmTransformS * ShoulderTransformW;

	float Roll = FMath::Max((LeftHandTransformW.GetTranslation() - LeftUpperArmTransformW.GetTranslation()).Z, 0.f);
	FTransform UpperArmS = LeftUpperArmTransformW * ComponentSpace;

	LeftUpperArmTransformC = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()), UpperArmS.GetTranslation());
	LeftLowerArmTransformC = LeftLowerArmTransformW * ComponentSpace;

	/** RIGHT ARM **/
	HandLoc = RightHandTransformS.GetTranslation();
	HandRot = RightHandTransformS.GetRotation().Rotator();
	UpperArmLoc = RightUpperArmTransformS.GetTranslation();

	SetElbowBasePosition(UpperArmLoc, HandLoc, false, UpperArmBase, LowerArmBase);

	BaseAngle = RotateElbowByHandPosition(HandLoc, false);

	UpperArm = FTransform::Identity;
	LowerArm = FTransform::Identity;

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, UpperArm, LowerArm);

	Angle = RotateElbowByHandRotation(LowerArm, HandRot);

	SafeAngle = UUBIK::SafeguardAngle(RightElbowHandAngle, Angle, 120.f);

	RightElbowHandAngle = FMath::FInterpTo(RightElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RightElbowHandAngle: %f"), RightElbowHandAngle);

	RotateElbow(RightElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, RightUpperArmTransformS, RightLowerArmTransformS);
	RightUpperArmTransformW = RightUpperArmTransformS * ShoulderTransformW;
	RightLowerArmTransformW = RightLowerArmTransformS * ShoulderTransformW;

	Roll = -1.f * FMath::Max((RightHandTransformW.GetTranslation() - RightUpperArmTransformW.GetTranslation()).Z, 0.f);
	UpperArmS = RightUpperArmTransformW * ComponentSpace;

	RightUpperArmTransformC = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()), UpperArmS.GetTranslation());
	RightLowerArmTransformC = RightLowerArmTransformW * ComponentSpace;
}

void FAnimNode_UBIKSolver::SetElbowBasePosition(FVector UpperArm, FVector Hand, bool IsLeftArm, FTransform& UpperArmTransform, FTransform& LowerArmTransform)
{
	float UpperArmToHandLen = (UpperArm - Hand).Size();
	
	float Sign = (IsLeftArm) ? (-1.f) : (1.f);

	float Beta = UUBIK::CosineRule(Settings.UpperArmLength, UpperArmToHandLen, Settings.LowerArmLength) * Sign;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Beta: %f"), Beta);

	float Omega = UUBIK::CosineRule(Settings.LowerArmLength, Settings.UpperArmLength, UpperArmToHandLen);
	Omega = (IsLeftArm) ? (180.f - Omega) : (180.f + Omega);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Omega: %f"), Omega);

	FRotator A = FQuat::FindBetweenNormals(FVector::ForwardVector, (Hand - UpperArm).GetSafeNormal()).Rotator();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("FindBetweenNormals: %s"), *A.ToString());
	FRotator B = FQuat(UKismetMathLibrary::GetUpVector(A), FMath::DegreesToRadians(Beta)).Rotator();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("AxisAngleRot: %s"), *B.ToString());

	UpperArmTransform = FTransform((UKismetMathLibrary::ComposeRotators(A, B)), UpperArm, FVector::OneVector);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("UpperArmTransform: %s"), *UpperArmTransform.ToString());

	FTransform TempLowerArm = FTransform(FRotator(0.f, Omega, 0.f), FVector::ForwardVector * Settings.UpperArmLength, FVector::OneVector);
	LowerArmTransform = TempLowerArm * UpperArmTransform;
	//UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArmTransform: %s"), *LowerArmTransform.ToString());
}

float FAnimNode_UBIKSolver::RotateElbowByHandPosition(FVector Hand, bool IsLeftArm)
{
	FVector TempHand = Hand / Settings.ArmLength;

	float Sign = (IsLeftArm) ? (1.f) : (-1.f);

	float Angle = Settings.ElbowBaseOffsetAngle +
		(Settings.ElbowYWeight * FMath::Max(0.f, (TempHand.Y * Sign) + Settings.ElbowYDistanceStart));

	return Angle;
}

float FAnimNode_UBIKSolver::RotateElbowByHandRotation(FTransform LowerArm, FRotator Hand)
{
	FRotator LowerArmRot = LowerArm.Rotator();

	FVector LowerArmRight = UKismetMathLibrary::GetRightVector(LowerArmRot);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("GetRightVector:: %s"), *LowerArmRight.ToString());

	FVector ProjectedForward = UKismetMathLibrary::ProjectVectorOnToPlane(
		UKismetMathLibrary::GetForwardVector(Hand), LowerArmRight
	).GetSafeNormal();
	//UE_LOG(LogUBIKRuntime, Display, TEXT("ProjectedForward:: %s"), *ProjectedForward.ToString());

	FVector LowerArmForward = UKismetMathLibrary::GetForwardVector(LowerArmRot);

	float TempAngle = UUBIK::CosineRule(1.f, 1.f, (ProjectedForward - LowerArmForward).Size());

	float Sign = (FVector::DotProduct(FVector::CrossProduct(ProjectedForward, LowerArmForward), LowerArmRight) < 0.f) ? (1.f) : (-1.f);
	TempAngle = TempAngle * Sign;
	TempAngle = TempAngle * Settings.ElbowRotFromHandRotAlpha;

	return TempAngle;
}

void FAnimNode_UBIKSolver::RotateElbow(float Angle, FTransform UpperArm, FTransform LowerArm, FVector HandLoc, bool IsLeftArm, FTransform& NewUpperArm, FTransform& NewLowerArm)
{
	FVector PivotLoc =
		UpperArm.GetTranslation() +
		UKismetMathLibrary::ProjectVectorOnToVector(UpperArm.GetTranslation() - LowerArm.GetTranslation(), UpperArm.GetTranslation() - HandLoc);

	FVector Forward = UpperArm.GetTranslation() - HandLoc;
	FVector Right = FVector::CrossProduct(UKismetMathLibrary::GetUpVector(UpperArm.Rotator()), Forward);
	FVector Up = UKismetMathLibrary::GetUpVector(UpperArm.Rotator());

	FRotator PivotRot = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, Up);

	float Roll = (IsLeftArm) ? (180.f - Angle) : (180.f + Angle);
	FRotator Delta = FRotator(0.f, 0.f, Roll);
	// UE_LOG(LogUBIKRuntime, Display, TEXT("!!! Delta: %s"), *Delta.ToString());

	FTransform Pivot = FTransform(PivotRot, PivotLoc, FVector::OneVector);
	// UE_LOG(LogUBIKRuntime, Display, TEXT("!!! Pivot: %s"), *Pivot.ToString()); --> Pivot is correct!

	NewUpperArm = UUBIK::RotatePointAroundPivot(UpperArm, Pivot, Delta);
	NewLowerArm = UUBIK::RotatePointAroundPivot(LowerArm, Pivot, Delta);
	// UE_LOG(LogUBIKRuntime, Display, TEXT("!!! NewUpperArm: %s - NewLowerArm: %s"), *NewUpperArm.ToString(), *NewLowerArm.ToString());

}

FTransform FAnimNode_UBIKSolver::GetBaseCharTransform()
{
	// I've already added the X and the Y component in SetShoulder(), that's why i negate these here.
	return FTransform(ShoulderTransformC.Rotator(), ShoulderTransformC.GetTranslation() + FVector(0.f, 0.f, Settings.BaseCharOffset.Z), FVector::OneVector);
}

void FAnimNode_UBIKSolver::DrawDebug()
{
	DebugDrawAxes(LeftHandTransformW, true);
	DebugDrawAxes(RightHandTransformW, true);
	DebugDrawAxes(LeftUpperArmTransformW, true);
	DebugDrawAxes(LeftLowerArmTransformW, true);
	//DrawDebugLine(GetWorld(), WorldTransform.GetLocation(), WorldTransform.GetLocation() + (WorldRot.GetUpVector() * 10.f), FColor::Blue);

}

void FAnimNode_UBIKSolver::DebugDrawAxes(FTransform Transform, bool bDrawAxes)
{
	// UE_LOG(LogUBIKRuntime, Display, TEXT("DrawAxes begin"));
	if (!MeshComponent)
		return;

	// UE_LOG(LogUBIKRuntime, Display, TEXT("DrawAxes MeshComponent ok"));
	World = MeshComponent->GetWorld();

	if (!World)
		return;

	// UE_LOG(LogUBIKRuntime, Display, TEXT("DrawAxes World ok"));
	DrawDebugSphere(World, Transform.GetTranslation(), 5.2f, 20.f, FColor::Silver);

	if (bDrawAxes)
	{
		FQuat WorldRot = Transform.GetRotation();
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetUpVector() * 10.f), FColor::Blue);
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetForwardVector() * 10.f), FColor::Red);
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetRightVector() * 10.f), FColor::Green);
	}
}


