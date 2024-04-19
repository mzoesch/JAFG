// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"

#include "UI/Common/CommonContainerSlot.h"
#include "CharacterInventorySlot.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UCharacterInventorySlot : public UCommonContainerSlot
{
    GENERATED_BODY()

protected:

    virtual void OnPrimaryClicked(void) override;
    virtual void OnSecondaryClicked(void) override;
};
