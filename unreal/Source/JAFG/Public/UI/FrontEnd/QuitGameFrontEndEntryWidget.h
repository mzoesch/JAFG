// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"

#include "Concretes/CommonBarEntryWidget.h"
#include "QuitGameFrontEndEntryWidget.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UQuitGameFrontEndEntryWidget : public UCommonBarEntryWidget
{
    GENERATED_BODY()

public:

    explicit UQuitGameFrontEndEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual auto OnThisTabPressed(void) const -> void override;
    virtual auto OnTabPressed(const FString& Identifier) -> void override;

    virtual auto OnPopUpClosed(const bool bAccepted) const -> void;
};
