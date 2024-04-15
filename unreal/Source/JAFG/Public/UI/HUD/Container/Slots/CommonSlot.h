// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "CommonSlot.generated.h"

JAFG_VOID

class USlateSlotData;
class UHotbar;
class UTextBlock;
class UImage;

UCLASS(Blueprintable)
class JAFG_API UCommonSlot : public UJAFGCommonWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

protected:

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USlateSlotData> SlateSlotData;

private:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UImage* I_Background;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UImage* I_Preview;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_Amount;

    void RenderSlot(void);
    friend UHotbar;
};
