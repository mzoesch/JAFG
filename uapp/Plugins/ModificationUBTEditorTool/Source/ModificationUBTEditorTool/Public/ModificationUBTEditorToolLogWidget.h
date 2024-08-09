// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModificationUBTEditorToolInstance.h"
#include "Widgets/SCompoundWidget.h"

class STableViewBase;
class FModificationUBTEditorToolInstance;

class MODIFICATIONUBTEDITORTOOL_API SModificationUBTEditorToolLogWidget final : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolLogWidget)
    {
    }
    SLATE_END_ARGS()

    SModificationUBTEditorToolLogWidget(void) = default;
    virtual ~SModificationUBTEditorToolLogWidget(void) override;

    auto Construct(const FArguments& InArgs) -> void;

    auto SetInstance(const TSharedPtr<FModificationUBTEditorToolInstance>& NewInstance) -> void;

protected:

    auto IsCancelButtonEnabled(void) const -> bool;
    auto IsRetryButtonEnabled(void) const -> bool;

    auto OnCancelButtonClicked(void) const -> FReply;
    auto OnRetryButtonClicked(void) -> FReply;
    auto OnClearLogButtonClicked(void) -> FReply;
    auto OnCopyLogButtonClicked(void) const -> FReply;
    auto OnSaveLogButtonClicked(void) -> FReply;

    auto GetPluginNameText(void) const -> FText;
    auto GetStatusText(void) const -> FText;
    auto GetStatusColor(void) const -> FSlateColor;
    auto GetStatusDescription(void) const -> FText;

    auto OnNewInstanceSpawned(const TSharedPtr<FModificationUBTEditorToolInstance>& NewInstance) -> void;

    auto OnInstanceMessage(const FModificationUBTEditorToolInstanceMessageEntry& Message) -> void;
    auto HandleMessageListViewGenerateRow(
        const TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry> Message,
        const TSharedRef<STableViewBase>& TableView
    ) const -> TSharedRef<ITableRow>;

    auto ClearMessageList(void) -> void;
    auto UpdateMessageListFromInstance(void) -> void;

    static auto VerbosityToColor(const ELogVerbosity::Type InVerbosity) -> FColor;

    TArray<TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry>> MessageList;
    TSharedPtr<FModificationUBTEditorToolInstance> Instance;
    TSharedPtr<SListView<TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry>>> MessageListView;

    FDelegateHandle OnInstanceMessageHandle;
    FDelegateHandle OnNewInstanceSpawnedHandle;

    FString LastLogFileSaveDirectory;
};
