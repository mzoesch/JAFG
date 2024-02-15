// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ContainerSlot.h"

#include "PlayerInventoryCrafterSlot.generated.h"

UCLASS()
class JAFG_API UW_PlayerInventoryCrafterSlot : public UW_ContainerSlot
{
    GENERATED_BODY()
    
public:
    
    virtual void OnClicked() override;
    virtual void OnSecondaryClicked() override;
};
