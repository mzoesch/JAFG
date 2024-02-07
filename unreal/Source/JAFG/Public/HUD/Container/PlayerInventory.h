// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "PlayerInventory.generated.h"

class UTileView;
class UW_PlayerInventoryCrafterResult;

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
	UW_PlayerInventoryCrafterResult* PlayerInventoryCrafterResult;
	
public:

	virtual void NativeOnInitialized() override;
	void OnInventoryUpdate();
};
