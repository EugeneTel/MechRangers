// 2020 Sticky Snout Studio (Jonas Molgaard)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "UBIK.h"
#include "AnimNode_UBIKSolver.generated.h"

/**
 * UBIK Solver
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** Feed in the HMD transform in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform HeadTransformW;
	
	/** Feed in the Left MotionController in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform LeftHandTransformW;
	
	/** Feed in the Right MotionController in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform RightHandTransformW;

	/** These settings will be returned by calling the GetUBIKSettings function. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FUBIKSettings Settings;

	/** Set this to get debug draws of certain internal transforms. Used only for debugging purposes. **/
	// TODO: Disabled for now, because it crashes engine for some reason.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	//bool bDrawDebug = false;

	//** Head bone to modify **/
	UPROPERTY(EditAnywhere, Category = HeadBone)
	FBoneReference HeadBoneToModify = FBoneReference("head");

	/** Left Clavicle bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference LeftClavicleBoneToModify = FBoneReference("clavicle_l");

	/** Right Clavicle Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference RightClavicleBoneToModify = FBoneReference("clavicle_r");

	/** Left Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference LeftUpperArmBoneToModify = FBoneReference("upperarm_l");

	/** Right Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference RightUpperArmBoneToModify = FBoneReference("upperarm_r");

	/** Left Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference LeftLowerArmBoneToModify = FBoneReference("lowerarm_l");;

	/** Right Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference RightLowerArmBoneToModify = FBoneReference("lowerarm_r");

	/** Left Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference LeftHandBoneToModify = FBoneReference("hand_l");

	/** Right Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference RightHandBoneToModify = FBoneReference("hand_r");

	//** Spine01 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine01_BoneToModify = FBoneReference("spine_01");

	//** Spine02 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine02_BoneToModify = FBoneReference("spine_02");

	//** Spine03 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine03_BoneToModify = FBoneReference("spine_03");

	//** Pelvis bone to modify **/
	UPROPERTY(EditAnywhere, Category = Pelvis)
	FBoneReference PelvisBoneToModify = FBoneReference("pelvis");

	/** By default Pelvis will be driven from Head location with an offset. Set to true to ignore Location. **/
	UPROPERTY(EditAnywhere, Category = Pelvis)
	bool bIgnoreLocation = false;

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	//virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	/** Internal use - Used for interps. */
	float DeltaTime;

	FTransform ComponentSpaceW;
	FTransform ShoulderTransformW;
	FTransform LeftUpperArmTransformW;
	FTransform RightUpperArmTransformW;
	FTransform LeftLowerArmTransformW;
	FTransform RightLowerArmTransformW;

	/** Must check if it's valid. Can be null. **/
	USkeletalMeshComponent* MeshComponent;
	UWorld* World;

	FTransform HeadTransformC;
	FTransform LeftHandTransformC;
	FTransform RightHandTransformC;
	FTransform ShoulderTransformC;
	FTransform LeftClavicleC;	// TODO: May be able to turn into a FRotator instead
	FTransform RightClavicleC;	// TODO: May be able to turn into a FRotator instead
	FTransform BaseCharTransformC;
	FTransform LeftUpperArmTransformC;
	FTransform LeftLowerArmTransformC;
	FTransform RightUpperArmTransformC;
	FTransform RightLowerArmTransformC;

	/** WorldSpace inverted **/
	FTransform ShoulderTransform;
	FTransform ComponentSpace;

	FTransform HeadTransformS;
	FTransform LeftHandTransformS;
	FTransform RightHandTransformS;
	FTransform LeftUpperArmTransformS;
	FTransform RightUpperArmTransformS;
	FTransform LeftLowerArmTransformS;
	FTransform RightLowerArmTransformS;

	float LeftHeadHandAngle;
	float RightHeadHandAngle;

	float LeftElbowHandAngle;
	float RightElbowHandAngle;

private:
	FRotator Head;
	FRotator Spine03;
	FRotator Spine02;
	FRotator Spine01;
	FTransform Pelvis;
	FRotator Clavicle_l;
	FRotator UpperArm_l;
	FRotator LowerArm_l;
	FRotator Hand_l;
	FRotator Clavicle_r;
	FRotator UpperArm_r;
	FRotator LowerArm_r;
	FRotator Hand_r;

private:
	void ConvertTransforms();
	void SetShoulder();
	FVector GetShoulderLocation();
	FRotator GetShoulderRotationFromHead();
	FRotator GetShoulderRotationFromHands();
	float GetHeadHandAngle(float LastAngle, FVector Hand, FVector HandHeadDelta);

	void SetLeftUpperArm();
	void SetRightUpperArm();
	FTransform RotateUpperArm(bool IsLeftArm, FVector HandTranslation);

	void ResetUpperArmsLocation();
	void SolveArms();
	void SetElbowBasePosition(FVector UpperArm, FVector Hand, bool IsLeftArm, FTransform& UpperArmTransform, FTransform& LowerArmTransform);
	float RotateElbowByHandPosition(FVector Hand, bool IsLeftArm);
	float RotateElbowByHandRotation(FTransform LowerArm, FRotator Hand);
	void RotateElbow(float Angle, FTransform UpperArm, FTransform LowerArm, FVector HandLoc, bool IsLeftArm, FTransform& NewUpperArm, FTransform& NewLowerArm);

	FTransform GetBaseCharTransform();

	void DrawDebug();
	void DebugDrawAxes(FTransform Transform, bool DrawAxis = true);

	/** The given Transform will only apply Translation if explicitly defined. **/
	void SetBoneTransform(const FBoneReference& BoneToModify, FTransform Transform, FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, bool bApplyRotation, bool bApplyTranslation = false);
};

