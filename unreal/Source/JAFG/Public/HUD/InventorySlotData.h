// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "InventorySlotData.generated.h"

UCLASS(BlueprintType)
class JAFG_API UInventorySlotData : public UObject
{
	GENERATED_BODY()

public:

	int Amount = 0;
	
};
