// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModificationUBTEditorToolModificationList.h"
#include "Widgets/SCompoundWidget.h"

class SModificationUBTEditorToolDevelopmentWidget final : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolDevelopmentWidget) { }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:

    TSharedPtr<IDetailsView> DetailsView;
    TSharedPtr<SModificationUBTEditorToolModificationList> PluginList;

    auto PackageAllMods(void) const -> FReply;
    auto CreatePlugin(void) const -> FReply;

    auto IsPackageButtonEnabled(void) const -> bool;
};
