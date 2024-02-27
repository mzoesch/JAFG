// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Voxel.generated.h"

class ACH_Master;
class UGI_Master;

UINTERFACE(MinimalAPI)
class UVoxel : public UInterface
{
	GENERATED_BODY()
};

class JAFG_API IVoxel
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void Initialize(UGI_Master* InGIPtr) = 0;

	UFUNCTION()
	virtual void OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed) = 0;
};
