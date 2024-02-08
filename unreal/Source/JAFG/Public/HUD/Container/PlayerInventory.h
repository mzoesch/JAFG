// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "PlayerInventory.generated.h"

class UTileView;
class UW_PlayerInventoryCrafterOutput;

UCLASS()
class JAFG_API UW_PlayerInventory : public UW_Master
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* PlayerInventorySlots;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* PlayerInventoryCrafterSlots;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UW_PlayerInventoryCrafterOutput* PlayerInventoryCrafterOutput;
	
public:

	virtual void NativeOnInitialized() override;
	/** Re-renders all tiles in the inventory screen including the actual character inventory,the crafter, etc. */
	void OnInventoryUpdate();
};
