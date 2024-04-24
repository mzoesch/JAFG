// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "JAFGCommonDirtyWidget.h"

#include "CommonContainer.generated.h"

class UCursorHandPreview;
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

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCursorHandPreview> W_CursorHand;

    auto RefreshCharacterInventorySlots(void) -> void;
    auto ResetCursorHand(void) -> void;
};
