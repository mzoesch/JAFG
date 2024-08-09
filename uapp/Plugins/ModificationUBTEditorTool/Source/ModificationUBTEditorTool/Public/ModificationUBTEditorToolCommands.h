// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FModificationUBTEditorToolCommands final : public TCommands<FModificationUBTEditorToolCommands>
{
public:

    FModificationUBTEditorToolCommands(void);

    virtual void RegisterCommands(void) override;

    TSharedPtr<FUICommandInfo> DevelopmentWidget;
    TSharedPtr<FUICommandInfo> ShippingWidget;
    TSharedPtr<FUICommandInfo> LogWidget;
};
