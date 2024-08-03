// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"

class UModificationUBTEditorToolMetadataObject;

class SModificationUBTEditorToolEditPluginDialogWidget final : public SWindow
{
public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolEditPluginDialogWidget) { }
    SLATE_END_ARGS()

    auto Construct(const FArguments& InArgs, TSharedRef<IPlugin> InPlugin) -> void;

private:

    auto UpdatePluginToCurrentlyUsedGameVersion(void) -> void;
    auto OnOkClicked(void) -> FReply;

    TSharedPtr<IPlugin> Plugin;
    UModificationUBTEditorToolMetadataObject* MetadataObject = nullptr;
};
