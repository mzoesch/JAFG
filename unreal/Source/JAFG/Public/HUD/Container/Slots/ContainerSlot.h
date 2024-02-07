// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "HUD/UW_Master.h"

#include "ContainerSlot.generated.h"

class UImage;
class UTextBlock;
class UContainerSlotData;

UCLASS()
class JAFG_API UW_ContainerSlot : public UW_Master, public IUserObjectListEntry
{
    GENERATED_BODY()
    
public:

    UPROPERTY(BlueprintReadOnly)
    UContainerSlotData* ContainerSlotData;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* SlotBackground;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* AccumulatedPreview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* AccumulatedAmount;

public:

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    void RenderSlot();
    
    UFUNCTION(BlueprintCallable)
    virtual void OnClicked() PURE_VIRTUAL(ContainerSlot::OnClicked);
    
};
