// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ContainerSlot.h"

#include "PlayerInventorySlot.generated.h"

class UInventorySlotData;
class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UW_PlayerInventorySlot : public UW_ContainerSlot
{
    GENERATED_BODY()

public:
    
    virtual void OnClicked() override;
};
