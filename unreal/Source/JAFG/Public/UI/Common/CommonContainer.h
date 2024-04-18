// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "JAFGCommonDirtyWidget.h"

#include "CommonContainer.generated.h"

JAFG_VOID

class UTileView;

UCLASS(Abstract, NotBlueprintable)
class JAFG_API UCommonContainer : public UJAFGCommonDirtyWidget
{
    GENERATED_BODY()

protected:

    virtual void NativeConstruct(void) override;
    virtual void OnRefresh(void) override;

private:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UTileView> TV_CharacterInventorySlots;

    void RefreshCharacterInventorySlots(void);
};
