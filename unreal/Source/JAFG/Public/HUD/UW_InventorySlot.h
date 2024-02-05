// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "HUD/UW_Master.h"

#include "UW_InventorySlot.generated.h"

class UInventorySlotData;
class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UUW_InventorySlot : public UUW_Master, public IUserObjectListEntry
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly)
    UInventorySlotData* InventorySlotData;
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* SlotBackground;
	
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* AccumulatedPreview;

public:
    
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    UFUNCTION(BlueprintCallable)
    void OnClick();
    
};
