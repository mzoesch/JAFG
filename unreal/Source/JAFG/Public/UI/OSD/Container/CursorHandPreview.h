// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "CursorHandPreview.generated.h"

JAFG_VOID

class UCanvasPanelSlot;
class UCanvasPanel;
class UTextBlock;
class UImage;

UCLASS(Abstract, Blueprintable)
class JAFG_API UCursorHandPreview : public UJAFGCommonWidget
{
    GENERATED_BODY()

private:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UCanvasPanel* CP_Wrapper;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCanvasPanelSlot* CPS_Wrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UImage* I_Preview;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_Amount;

protected:

    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) -> void override;
};
