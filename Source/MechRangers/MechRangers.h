// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_VR_TRACE      ECC_GameTraceChannel1
#define COLLISION_MECH    		ECC_GameTraceChannel2
#define COLLISION_ENEMY     	ECC_GameTraceChannel3
#define COLLISION_PROJECTILE	ECC_GameTraceChannel4
#define COLLISION_WEAPON_TRACE  ECC_GameTraceChannel5
