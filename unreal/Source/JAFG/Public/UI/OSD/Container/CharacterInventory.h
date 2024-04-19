// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/Common/CommonContainer.h"

#include "CharacterInventory.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UCharacterInventory : public UCommonContainer
{
    GENERATED_BODY()

protected:

    virtual void NativeConstruct(void) override;
    virtual void OnRefresh(void) override;

private:

    FDelegateHandle OnInventoryChanged_ClientDelegateHandle;

public:

    void Toggle(const bool bCollapsed);
};
