// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWidget.h"

#include "QuickSessionPreviewWidget.generated.h"

JAFG_VOID

class UHorizontalBox;

UCLASS(Abstract, Blueprintable)
class JAFG_API UQuickSessionPreviewWidget : public UJAFGWidget
{
    GENERATED_BODY()

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    // ~UUserWidget implementation

    virtual auto UpdateWidget(void) -> void;

    FDelegateHandle QuickSessionPreviewVisibilityChangedHandle;
    virtual auto OnQuickSessionPreviewVisibilityChanged(const bool bVisible) -> void;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UHorizontalBox* HB_SessionPreviewContainer;
};
