// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolLogWidget.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorTool"

SModificationUBTEditorToolLogWidget::~SModificationUBTEditorToolLogWidget(void)
{
    if (this->OnNewInstanceSpawnedHandle.IsValid())
    {
        FModificationUBTEditorToolInstance::OnNewInstanceSpawnedDelegate.Remove(this->OnNewInstanceSpawnedHandle);
        this->OnNewInstanceSpawnedHandle.Reset();
    }

    if (this->OnInstanceMessageHandle.IsValid() && this->Instance.IsValid())
    {
        this->Instance->OnMessageReceived().Remove(this->OnInstanceMessageHandle);
        this->OnInstanceMessageHandle.Reset();
    }

    return;
}

void SModificationUBTEditorToolLogWidget::Construct(const FArguments& InArgs)
{
    this->ChildSlot[
        SNew(SVerticalBox)
        + SVerticalBox::Slot().HAlign(HAlign_Left).Padding(5.0f, 10.0f, 5.0f, 2.0f).AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth()
            [
                SNew(STextBlock)
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                .Text(LOCTEXT("LogPluginName", "Plugin: "))
            ]
            + SHorizontalBox::Slot().AutoWidth()
            [
                SNew(STextBlock)
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                .Text(this, &SModificationUBTEditorToolLogWidget::GetPluginNameText)
            ]
        ]
        + SVerticalBox::Slot().HAlign(HAlign_Left).Padding(5.0f, 0.0f, 5.0f, 2.0f).AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth()
            [
                SNew(STextBlock)
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                .Text(LOCTEXT("LogStatusColumn", "Status: "))
            ]
            + SHorizontalBox::Slot().AutoWidth()
            [
                SNew(STextBlock)
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
                .ColorAndOpacity(this, &SModificationUBTEditorToolLogWidget::GetStatusColor)
                .Text(this, &SModificationUBTEditorToolLogWidget::GetStatusText)
            ]
        ]
        + SVerticalBox::Slot().HAlign(HAlign_Left).Padding(5.0f, 0.0f, 5.0f, 0.0f).AutoHeight()
        [
            SNew(STextBlock)
            .TextStyle(FAppStyle::Get(), "SmallText")
            .Text(this, &SModificationUBTEditorToolLogWidget::GetStatusDescription)
        ]
        + SVerticalBox::Slot().HAlign(HAlign_Fill).Padding(5.0f, 5.0f, 5.0f, 0.0f).FillHeight(1.0f)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            .Padding(2.0f)
            [
                SAssignNew(this->MessageListView, SListView<TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry>>)
                .HeaderRow
                (
                    SNew(SHeaderRow)
                    .Visibility(EVisibility::Collapsed)
                    + SHeaderRow::Column("Status")
                    .DefaultLabel(LOCTEXT("TaskListOutputLogColumnHeader", "Output Log"))
                )
                .ListItemsSource(&this->MessageList)
                .OnGenerateRow(this, &SModificationUBTEditorToolLogWidget::HandleMessageListViewGenerateRow)
                .ItemHeight(24.0)
                .SelectionMode(ESelectionMode::Multi)
                .AllowOverscroll(EAllowOverscroll::No)
                .ConsumeMouseWheel(EConsumeMouseWheel::Always)
            ]
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(FMargin(20.0f, 10.0f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().HAlign(HAlign_Center).AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(FMargin(6.0f, 2.0f))
                .IsEnabled_Lambda( [this] { return this->MessageListView->GetNumItemsSelected() > 0; })
                .Text(LOCTEXT("CopyButtonText", "Copy"))
                .ToolTipText(LOCTEXT("CopyButtonTooltip", "Copy to clipboard."))
                .OnClicked(this, &SModificationUBTEditorToolLogWidget::OnCopyLogButtonClicked)
            ]
            + SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(4.0f, 0.0f, 0.0f, 0.0f).AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(FMargin(6.0f, 2.0f))
                .IsEnabled_Lambda( [this] { return this->MessageList.IsEmpty() == false; })
                .Text(LOCTEXT("ClearButtonText", "Clear Log"))
                .OnClicked(this, &SModificationUBTEditorToolLogWidget::OnClearLogButtonClicked)
            ]
            + SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(4.0f, 0.0f, 0.0f, 0.0f).AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(FMargin(6.0f, 2.0f))
                .IsEnabled_Lambda( [this] { return this->MessageList.IsEmpty() == false; })
                .Text(LOCTEXT("ExportButtonText", "Save Log..."))
                .ToolTipText(LOCTEXT("SaveButtonTooltip", "Save entire log to file."))
                .OnClicked(this, &SModificationUBTEditorToolLogWidget::OnSaveLogButtonClicked)
            ]
            + SHorizontalBox::Slot().FillWidth(1)
            + SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(0.0f, 0.0f, 10.0f, 0.0f).AutoWidth()
            [
                SNew(SButton)
                .Text(LOCTEXT("ButtonRetry", "Run Again"))
                .ToolTipText(LOCTEXT("ButtonRetryTooltip", "Retry the Packaging Task with the same Parameters."))
                .IsEnabled(this, &SModificationUBTEditorToolLogWidget::IsRetryButtonEnabled)
                .OnClicked(this, &SModificationUBTEditorToolLogWidget::OnRetryButtonClicked)
            ]
            + SHorizontalBox::Slot().HAlign(HAlign_Center).AutoWidth()
            [
                SNew(SButton)
                .Text(LOCTEXT("ButtonCancel", "Cancel"))
                .ToolTipText(LOCTEXT("ButtonCancelTooltip", "Cancels the currently running Packaging Task."))
                .IsEnabled(this, &SModificationUBTEditorToolLogWidget::IsCancelButtonEnabled)
                .OnClicked(this, &SModificationUBTEditorToolLogWidget::OnCancelButtonClicked)
            ]
        ]
    ];

    const TWeakPtr<SModificationUBTEditorToolLogWidget> WeakSelf =
        StaticCastWeakPtr<SModificationUBTEditorToolLogWidget>(this->AsWeak());
    FModificationUBTEditorToolInstance::OnNewInstanceSpawned().AddLambda(
    [WeakSelf] (const TSharedPtr<FModificationUBTEditorToolInstance>& NewInstance)
    {
        if (const TSharedPtr<SModificationUBTEditorToolLogWidget> SharedSelf = WeakSelf.Pin())
        {
            SharedSelf->OnNewInstanceSpawned(NewInstance);
        }

        return;
    });

    return;
}

void SModificationUBTEditorToolLogWidget::SetInstance(const TSharedPtr<FModificationUBTEditorToolInstance>& NewInstance)
{
    if (this->Instance != NewInstance)
    {
        if (this->Instance.IsValid())
        {
            this->Instance->OnMessageReceived().Remove(this->OnInstanceMessageHandle);
            this->OnInstanceMessageHandle.Reset();
        }

        this->Instance = NewInstance;
        this->UpdateMessageListFromInstance();
    }

    return;
}

bool SModificationUBTEditorToolLogWidget::IsCancelButtonEnabled(void) const
{
    return this->Instance.IsValid() && this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Running;
}

bool SModificationUBTEditorToolLogWidget::IsRetryButtonEnabled(void) const
{
    return this->Instance.IsValid() && this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed;
}

FReply SModificationUBTEditorToolLogWidget::OnCancelButtonClicked(void) const
{
    if (this->Instance.IsValid() && this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Running)
    {
        this->Instance->Cancel();
    }

    return FReply::Handled();
}

FReply SModificationUBTEditorToolLogWidget::OnRetryButtonClicked(void)
{
    if (this->Instance.IsValid() && this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed)
    {
        ClearMessageList();
        this->Instance->Start();
    }

    return FReply::Handled();
}

FReply SModificationUBTEditorToolLogWidget::OnClearLogButtonClicked(void)
{
    this->MessageList.Reset();
    this->MessageListView->RequestListRefresh();

    return FReply::Handled();
}

FReply SModificationUBTEditorToolLogWidget::OnCopyLogButtonClicked(void) const
{
    if (
        TArray<TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry>> SelectedItems = this->MessageListView->GetSelectedItems();
        SelectedItems.Num() > 0
    )
    {
        FString SelectedText;

        for (int32 i = 0; i < SelectedItems.Num(); ++i)
        {
            SelectedText += SelectedItems[i]->Message;
            SelectedText += LINE_TERMINATOR;
        }

        FPlatformApplicationMisc::ClipboardCopy(*SelectedText);
    }

    return FReply::Handled();
}

FReply SModificationUBTEditorToolLogWidget::OnSaveLogButtonClicked(void)
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(this->AsShared());
    const void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
        ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
        : nullptr;

    FDateTime Timestamp = FDateTime::Now();
    if (this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed)
    {
        Timestamp = this->Instance->GetEndTime();
    }

    if (
        TArray<FString> Filenames;
        DesktopPlatform->SaveFileDialog(
            ParentWindowHandle,
            LOCTEXT("SaveLogDialogTitle", "Save Log As...").ToString(),
            this->LastLogFileSaveDirectory,
            FString::Printf(TEXT("Alpakit-%s-%s.log"),
            *this->Instance->GetPluginName(),
            *Timestamp.ToString(TEXT("%Y-%m-%d-%H-%M-%S"))),
            TEXT("Log Files (*.log)|*.log"),
            EFileDialogFlags::None,
            Filenames
        )
    )
    {
        if (Filenames.Num() > 0)
        {
            FString Filename = Filenames[0];

            this->LastLogFileSaveDirectory = FPaths::GetPath(Filename);

            if (FPaths::GetExtension(Filename).IsEmpty())
            {
                Filename += Filename + TEXT(".log");
            }

            if (
                FArchive* LogFile = IFileManager::Get().CreateFileWriter(*Filename);
                LogFile != nullptr
            )
            {
                for (int32 i = 0; i < this->MessageList.Num(); ++i)
                {
                    FString LogEntry = this->MessageList[i]->Message + LINE_TERMINATOR;
                    LogFile->Serialize(TCHAR_TO_ANSI(*LogEntry), LogEntry.Len());
                }

                LogFile->Close();

                delete LogFile;
            }
            else
            {
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SaveLogDialogFileError", "Failed to open the specified file for saving."));
            }
        }
    }

    return FReply::Handled();
}

FText SModificationUBTEditorToolLogWidget::GetPluginNameText(void) const
{
    if (
           this->Instance.IsValid() == false
        || this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::None
    )
    {
        return LOCTEXT("PluginNameNA", "N/A");
    }
    return FText::FromString(this->Instance->GetPluginName());
}

FText SModificationUBTEditorToolLogWidget::GetStatusText(void) const
{
    if (this->Instance.IsValid() == false || this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::None)
    {
        return LOCTEXT("StatusIdle", "Idle");
    }
    if (this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed)
    {
        const EModificationUBTEditorToolInstanceResult::Type Result = this->Instance->GetResult();

        FText ResultText;

        if (Result == EModificationUBTEditorToolInstanceResult::Success)
        {
            ResultText = LOCTEXT("StatusCompleted", "Completed.");
        }
        if (Result == EModificationUBTEditorToolInstanceResult::Fail)
        {
            ResultText = LOCTEXT("StatusFailed", "Failed.");
        }
        if  (Result == EModificationUBTEditorToolInstanceResult::Cancelled)
        {
            ResultText = LOCTEXT("StatusCancelled", "Cancelled by user.");
        }

        const FTimespan Delta = FDateTime::UtcNow() - this->Instance->GetEndTime();

        return FText::Format(
            LOCTEXT("StatusAt", "{0} at {1} ({2} ago)"),
            ResultText,
            FText::AsDateTime(this->Instance->GetEndTime()),
            FText::AsTimespan(Delta)
        );
    }

    return LOCTEXT("StatusRunning", "Running...");
}

FSlateColor SModificationUBTEditorToolLogWidget::GetStatusColor(void) const
{
    if (this->Instance.IsValid() == false || this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::None)
    {
        return FSlateColor(FColor::White);
    }

    if (this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed)
    {
        return FSlateColor(this->Instance->GetResult() == EModificationUBTEditorToolInstanceResult::Success
            ? FColor::Green
            : FColor::Red
        );
    }

    return FSlateColor(FColor::Yellow);
}

FText SModificationUBTEditorToolLogWidget::GetStatusDescription(void) const
{
    if (this->Instance.IsValid() == false || this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::None)
    {
        return LOCTEXT("StatusDescNone", "Start a new package action from Modification UBT Editor Tool Menu to continue.");
    }

    if (this->Instance->GetInstanceState() == EModificationUBTEditorToolInstanceState::Completed)
    {
        const EModificationUBTEditorToolInstanceResult::Type Result = this->Instance->GetResult();

        if (Result == EModificationUBTEditorToolInstanceResult::Success)
        {
            return LOCTEXT("StatusDescCompleted", "The packaging action has been completed successfully.");
        }
        if (Result == EModificationUBTEditorToolInstanceResult::Fail)
        {
            return LOCTEXT("StatusDescFailed", "The packaging action has finished with an error. See Output Log for details.");
        }
        if (Result == EModificationUBTEditorToolInstanceResult::Cancelled)
        {
            return LOCTEXT("StatusDescCancelled", "The packaging action has been cancelled.");
        }
    }

    return LOCTEXT("StatusDescRunning", "The packaging action is currently running.");
}

void SModificationUBTEditorToolLogWidget::OnNewInstanceSpawned(const TSharedPtr<FModificationUBTEditorToolInstance>& NewInstance)
{
    if (
           this->Instance.IsValid() == false
        || this->Instance->GetInstanceState() != EModificationUBTEditorToolInstanceState::Running
    )
    {
        this->SetInstance(NewInstance);
    }

    return;
}

void SModificationUBTEditorToolLogWidget::OnInstanceMessage(const FModificationUBTEditorToolInstanceMessageEntry& Message)
{
    this->MessageList.Add(MakeShared<FModificationUBTEditorToolInstanceMessageEntry>(Message));

    /* Only scroll when at the end of the list view. */
    const bool bShouldScroll =
           this->MessageListView->IsScrollbarNeeded() == false
        || FMath::IsNearlyEqual(this->MessageListView->GetScrollDistanceRemaining().Y, 0.0f, 1.e-7f);

    this->MessageListView->RequestListRefresh();

    if (bShouldScroll)
    {
        this->MessageListView->RequestScrollIntoView(this->MessageList.Last());
    }

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
TSharedRef<ITableRow> SModificationUBTEditorToolLogWidget::HandleMessageListViewGenerateRow(
    /* This is a wrong warning, as the slate framework requires this specific function signature. */
    // ReSharper disable once CppPassValueParameterByConstReference
    const TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry> Message,
    const TSharedRef<STableViewBase>& TableView
) const
{
    return SNew(STableRow<TSharedPtr<FModificationUBTEditorToolInstanceMessageEntry>>, TableView)
        .Content()
        [
            SNew(STextBlock)
            .TextStyle(FAppStyle::Get(), "SmallText")
            .ColorAndOpacity(this->VerbosityToColor(Message->Verbosity))
            .Text(FText::FromString(Message->Message))
        ];
}

void SModificationUBTEditorToolLogWidget::ClearMessageList(void)
{
    this->MessageList.Empty();
    this->MessageListView->RequestListRefresh();

    return;
}

void SModificationUBTEditorToolLogWidget::UpdateMessageListFromInstance(void)
{
    this->MessageList.Empty();
    if (this->Instance.IsValid())
    {
        const TWeakPtr<SModificationUBTEditorToolLogWidget> WeakSelf = StaticCastWeakPtr<SModificationUBTEditorToolLogWidget>(this->AsWeak());
        this->OnInstanceMessageHandle = this->Instance->OnMessageReceived().AddLambda(
        [WeakSelf] (const FModificationUBTEditorToolInstanceMessageEntry& Message)
        {
            if (const TSharedPtr<SModificationUBTEditorToolLogWidget> SharedSelf = WeakSelf.Pin())
            {
                SharedSelf->OnInstanceMessage(Message);
            }
        });

        for (const FModificationUBTEditorToolInstanceMessageEntry& ExistingMessage : this->Instance->GetMessageList())
        {
            this->MessageList.Add(MakeShared<FModificationUBTEditorToolInstanceMessageEntry>(ExistingMessage));
        }
    }

    this->MessageListView->RequestListRefresh();
    if (this->MessageList.Num() > 0)
    {
        this->MessageListView->RequestScrollIntoView(this->MessageList.Last());
    }

    return;
}

FColor SModificationUBTEditorToolLogWidget::VerbosityToColor(const ELogVerbosity::Type InVerbosity)
{
    switch (InVerbosity)
    {
    case ELogVerbosity::Fatal:
    case ELogVerbosity::Error:
    {
        return FColor::Red;
    }
    case ELogVerbosity::Warning:
    {
        return FColor::Yellow;
    }
    default:
    {
        return FColor::White;
    }
    }
}

#undef LOCTEXT_NAMESPACE
