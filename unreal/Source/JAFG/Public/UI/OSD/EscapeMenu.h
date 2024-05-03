// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWidget.h"

#include "EscapeMenu.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenu : public UJAFGWidget
{
    GENERATED_BODY()

public:

    explicit UEscapeMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    FDelegateHandle EscapeMenuVisibilityChangedDelegateHandle;
    virtual void OnEscapeMenuVisibilityChanged(const bool bVisible);
};
