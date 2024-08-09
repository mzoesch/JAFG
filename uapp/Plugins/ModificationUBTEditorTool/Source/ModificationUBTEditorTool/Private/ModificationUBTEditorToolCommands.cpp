// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolCommands.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorTool"

FModificationUBTEditorToolCommands::FModificationUBTEditorToolCommands() : TCommands<FModificationUBTEditorToolCommands>(
    TEXT("ModificationUBTEditorTool"),
    NSLOCTEXT("Contexts", "ModificationUBTEditorTool", "ModificationUBTEditorTool Plugin"),
    NAME_None,
    "EditorStyle"
)
{
    return;
}

void FModificationUBTEditorToolCommands::RegisterCommands(void)
{
    UI_COMMAND(
        DevelopmentWidget,
        "Development Widget",
        "Open the development widget.",
        EUserInterfaceActionType::Button,
        FInputChord { }
    );

    UI_COMMAND(
        ShippingWidget,
        "Shipping Widget",
        "Open the shipping widget.",
        EUserInterfaceActionType::Button,
        FInputChord { }
    );

    UI_COMMAND(
        LogWidget,
        "Log Widget",
        "Open the log widget.",
        EUserInterfaceActionType::Button,
        FInputChord { }
    )

    return;
}

#undef LOCTEXT_NAMESPACE
