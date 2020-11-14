// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MRMechController.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMRMechController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** god mode cheat */
	UPROPERTY(Transient)
	bool bGodMode;

public:
	
	/** get gode mode cheat */
	bool HasGodMode() const;
	
};
