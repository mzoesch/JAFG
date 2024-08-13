// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Foundation/Blueprints/CommonJAFGDualContainer.h"

#include "PlayerInventory.generated.h"

class IContainerCrafter;
class UJAFGContainerCrafterProductSlot;

UCLASS(Abstract, Blueprintable)
class JAFG_API UPlayerInventory : public UCommonJAFGDualContainer
{
    GENERATED_BODY()

public:

    explicit UPlayerInventory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    inline static const FString Identifier = TEXT("PlayerInventory");

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGContainerCrafterProductSlot> Slot_InventoryCrafterProduct;

    auto GetOtherContainerAsCrafter(void) const -> IContainerCrafter*;
};
