// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "UW_Inventory.generated.h"

class UUW_InventorySlotV2;
class UUW_HotbarSlot;
class UTileView;

UCLASS()
class JAFG_API UUW_Inventory : public UUW_Master
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* TileView;

public:

	void NativeOnInitialized() override;
	void OnInventoryUpdate();
};
