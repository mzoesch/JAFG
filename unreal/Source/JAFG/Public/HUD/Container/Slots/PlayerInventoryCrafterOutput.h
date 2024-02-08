// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ContainerSlot.h"

#include "PlayerInventoryCrafterOutput.generated.h"

UCLASS()
class JAFG_API UW_PlayerInventoryCrafterOutput : public UW_ContainerSlot
{
    GENERATED_BODY()

public:

    virtual void OnClicked() override;
    
};
