// Copyright 2024 mzoesch. All rights reserved.

#include "ChatMenu.h"
#include "ChatComponentImpl.h"
#include "ChatStatics.h"
#include "EnhancedInputSubsystems.h"
#include "JAFGSlateSettings.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Input/CustomInputNames.h"
#include "Input/JAFGEditableText.h"
#include "SettingsData/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
#include "JAFGLogDefs.h"
#include "JAFGSettingsLocal.h"
#include "Blueprint/WidgetTree.h"
#include "Commands/ChatCommandStatics.h"
#include "Commands/ShippedWorldChatCommands.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGTextBlock.h"

#define OWNING_PLAYER_CONTROLLER                              \
    Cast<AWorldPlayerController>(this->GetOwningPlayer())
#define OWNING_CHAT_COMPONENT                                 \
    Cast<UChatComponentImpl>(                                 \
        Cast<AWorldPlayerController>(this->GetOwningPlayer()  \
    )->GetComponentByClass(UChatComponentImpl::StaticClass()))

void UChatMenuEntry::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FChatMessagePassData)
    {
        this->MessageData = *Data;
    }

    this->ConstructMessage();

    return;
}

void UChatMenuEntry::ConstructMessage(void)
{
    this->TextBlock_Message->SetColorAndOpacity(
        LexToSlateColor(UJAFGSettingsLocal::Get(), this->MessageData.Message.Type, this->MessageData.Message.Format)
    );

    if (this->MessageData.Message.Sender.IsEmpty())
    {
        check( this->MessageData.Message.Type != EChatMessageType::Player )
        this->TextBlock_Message->SetText(this->MessageData.Message.Message);
    }
    else
    {
        this->TextBlock_Message->SetText(FText::FromString(FString::Printf(
            TEXT("<%s> %s"),
            *this->MessageData.Message.Sender, *this->MessageData.Message.Message.ToString()
        )));
    }

    return;
}

UChatMenu::UChatMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UChatMenu::NativeConstruct(void)
{
    Super::NativeConstruct();

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        LOG_ERROR(LogCommonSlate, "Detected editor simulation mode. But HUD decided to spawn this widget anyway.")
        return;
    }
#endif /* WITH_EDITOR */

    AWorldPlayerController* Owner = OWNING_PLAYER_CONTROLLER;

    if (Owner == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController but HUD decided to spawn this widget anyway.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
        return;
    }

    if (OWNING_CHAT_COMPONENT == nullptr)
    {
        LOG_RELAXED_FATAL(LogJAFGChat, "Chat component is not attached to the owning player controller.")
    }

    this->PreviewEntries.Empty();

    this->ChatMenuVisibilityChangedHandle = Owner->SubscribeToChatVisibilityChanged(ADD_SLATE_VIS_DELG(UChatMenu::OnChatMenuVisibilityChanged));
    this->ChatHistoryLookupHandle         = Owner->SubscribeToChatHistoryLookup(FChatHistoryLookupSignature::FDelegate::CreateUObject(this, &UChatMenu::OnHistoryLookUp));

    this->EditableText_StdIn->SetCustomEventToKeyDown(this->GetOnStdInKeyDownHandler());
    this->EditableText_StdIn->OnTextChanged.AddDynamic(this, &UChatMenu::OnChatTextChanged);
    this->EditableText_StdIn->OnTextCommitted.AddDynamic(this, &UChatMenu::OnChatTextCommitted);

    this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);

    /* For messages that have arrived while this widget has not yet been constructed. */
    while (OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.IsEmpty() == false)
    {
        FChatMessage PreConstruct;
        OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.Dequeue(PreConstruct);

        this->AddMessageToChatLog(PreConstruct);
    }

    // UFontSubsystem* FontSubsystem = this->GetWorld()->GetGameInstance()->GetSubsystem<UFontSubsystem>();
    // check( FontSubsystem )
    //
    //
    // FSlateFontInfo FontInfo;
    // FontInfo.CompositeFont = MakeShared<FCompositeFont>(FontSubsystem->CoreFontData.Font->CompositeFont);
    // this->EditableText_StdIn->SetFont(FontInfo);
    //
    // // Temp
    // this->TempTable = FontSubsystem->CoreFontData.ConstructNewMinimalDataTable();
    // URichTextBlock* RichTextBlock = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass());
    // RichTextBlock->SetText(FText::FromString(TEXT("Test Test Test")));
    // RichTextBlock->SetTextStyleSet(this->TempTable);
    // this->Overlay_OutWrapper->AddChild(RichTextBlock);

    return;
}

void UChatMenu::NativeDestruct(void)
{
    Super::NativeDestruct();

    if (OWNING_PLAYER_CONTROLLER == nullptr)
    {
        return;
    }

    if (OWNING_PLAYER_CONTROLLER->UnSubscribeToChatVisibilityChanged(this->ChatMenuVisibilityChangedHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Chat Visibility Changed event.")
    }

    if (OWNING_PLAYER_CONTROLLER->UnSubscribeToChatHistoryLookup(this->ChatHistoryLookupHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Chat History Lookup event.")
    }

    return;
}

void UChatMenu::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (const EChatMenuVisibility::Type CurrentVisibility = this->GetChatMenuVisibility(); CurrentVisibility == EChatMenuVisibility::Preview)
    {
        this->RemoveOutdatedPreviewEntries();
        this->ChangeChatMenuVisibilityStateBasedOnPreviewEntries();
    }

    return;
}

void UChatMenu::AddMessageToChatLog(const EChatMessageType::Type Type, const EChatMessageFormat::Type FormatType, const FString& Sender, const FText& Message)
{
    this->AddMessageToChatLog(FChatMessage(Type, FormatType, Sender, Message));
}

void UChatMenu::AddMessageToChatLog(const EChatMessageType::Type Type, const FString& Sender, const FText& Message)
{
    this->AddMessageToChatLog(FChatMessage(Type, Sender, Message));
}

void UChatMenu::AddMessageToChatLog(const FString& Sender, const FText& Message)
{
    this->AddMessageToChatLog(FChatMessage(Sender, Message));
}

void UChatMenu::AddMessageToChatLog(const FChatMessage& Message)
{
    this->ScrollBox_StdOut->AddChild(this->ConstructChatMenuEntry(Message));
    this->SafeAddToPreviewOut(Message);

    if (this->GetChatMenuVisibility() == EChatMenuVisibility::Full)
    {
        if (this->ScrollBox_StdOut->GetScrollOffsetOfEnd() - this->ScrollBox_StdOut->GetScrollOffset() < this->StdOutScrollOffsetOfEndIgnoreDelta)
        {
            /* Give the scroll bar time to render. And execute on the next tick. */
            AsyncTask(ENamedThreads::GameThread, [this] (void) -> void
            {
                this->ScrollBox_StdOut->ScrollToEnd();
            });
        }
        return;
    }

    this->ChangeChatMenuVisibility(EChatMenuVisibility::Preview);

    return;
}

void UChatMenu::ClearAllChatEntries(void)
{
    this->ScrollBox_StdOut->ClearChildren();
    this->VerticalBox_PreviewOut->ClearChildren();
    this->PreviewEntries.Empty();

    return;
}

void UChatMenu::OnChatMenuVisibilityChanged(const bool bVisible)
{
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( Subsystem )

    UJAFGInputSubsystem* JAFGSubsystem = ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( JAFGSubsystem )

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(bVisible ? InputContexts::Chat : InputContexts::FootWalk), 0);

    if (bVisible)
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Chat is now visible.")
        this->ChangeChatMenuVisibility(EChatMenuVisibility::Full);
    }
    else
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Chat is now hidden.")
        this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);
    }

    return;
}

void UChatMenu::HideChatMenu(void)
{
    OWNING_PLAYER_CONTROLLER->OnToggleChat(FInputActionValue());
}

void UChatMenu::ChangeChatMenuVisibility(const EChatMenuVisibility::Type InVisibility)
{
    if (this->GetChatMenuVisibility() == InVisibility)
    {
        return;
    }

    switch (InVisibility)
    {
    case EChatMenuVisibility::Collapsed:
    {
        if (this->PreviewEntries.IsEmpty() == false)
        {
            this->ChangeChatMenuVisibility(EChatMenuVisibility::Preview);
            return;
        }

        this->SetVisibility(ESlateVisibility::Collapsed);
        this->VerticalBox_PreviewOut->ClearChildren();
        this->PreviewEntries.Empty();
        this->ClearStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
        break;
    }
    case EChatMenuVisibility::Preview:
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->PanelWidget_StdOutWrapper->SetVisibility(ESlateVisibility::Collapsed);
        this->PanelWidget_PreviewOutWrapper->SetVisibility(ESlateVisibility::Visible);
        this->Border_StdInWrapper->SetVisibility(ESlateVisibility::Hidden);
        this->PanelWidget_CmdSuggestionsWrapper->SetVisibility(ESlateVisibility::Collapsed);

        this->ClearStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;

        break;
    }
    case EChatMenuVisibility::Full:
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->PanelWidget_StdOutWrapper->SetVisibility(ESlateVisibility::Visible);
        this->PanelWidget_PreviewOutWrapper->SetVisibility(ESlateVisibility::Collapsed);
        this->Border_StdInWrapper->SetVisibility(ESlateVisibility::Visible);
        this->PanelWidget_CmdSuggestionsWrapper->SetVisibility(ESlateVisibility::Visible);

        this->ScrollBox_StdOut->ScrollToEnd();
        this->ClearStdIn();
        this->FocusStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
        this->VerticalBox_CmdSuggestions->ClearChildren();

        if (this->ScrollBox_StdOut->GetAllChildren().IsEmpty())
        {
            this->PanelWidget_StdOutWrapper->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            this->PanelWidget_StdOutWrapper->SetVisibility(ESlateVisibility::Visible);
        }

        break;
    }
    default:
    {
        LOG_FATAL(LogJAFGChat, "Unknown chat menu visibility type: %d.", static_cast<int32>(InVisibility))
        break;
    }
    }

    return;
}

EChatMenuVisibility::Type UChatMenu::GetChatMenuVisibility(void) const
{
    if (this->GetVisibility() == ESlateVisibility::Collapsed)
    {
        return EChatMenuVisibility::Collapsed;
    }

    if (this->PanelWidget_PreviewOutWrapper->GetVisibility() == ESlateVisibility::Visible)
    {
        return EChatMenuVisibility::Preview;
    }

    return EChatMenuVisibility::Full;
}

void UChatMenu::SafeAddToPreviewOut(const FChatMessage& Message)
{
    UUserWidget* Entry = this->ConstructChatMenuEntry(Message);
    this->VerticalBox_PreviewOut->AddChild(Entry);

    FPrivatePreviewEntryData Data;
    Data.CreationTimeInSeconds = this->GetWorld()->GetTimeSeconds();
    Data.Entry                 = Entry;

    this->PreviewEntries.Emplace(Data);

    while (this->VerticalBox_PreviewOut->GetChildrenCount() > this->MaxEntriesInPreviewOut)
    {
        UUserWidget* EntryToRemove = Cast<UUserWidget>(this->VerticalBox_PreviewOut->GetChildAt(0));
        this->PreviewEntries.RemoveAll(
            [EntryToRemove] (const FPrivatePreviewEntryData& ExistingData) -> bool
            {
                return ExistingData.Entry == EntryToRemove;
            }
        );
        this->VerticalBox_PreviewOut->RemoveChildAt(0);
    }

    return;
}

void UChatMenu::RemoveOutdatedPreviewEntries(void)
{
    this->PreviewEntries.RemoveAll(
        [this] (const FPrivatePreviewEntryData& Data) -> bool
        {
            if (this->GetWorld()->GetTimeSeconds() - Data.CreationTimeInSeconds >= this->PreviewOutEntryLifetimeInSeconds)
            {
                if (Data.Entry == nullptr || Data.Entry->IsValidLowLevel() == false)
                {
                    return true;
                }

                this->VerticalBox_PreviewOut->RemoveChild(Data.Entry);
                return true;
            }

            return false;
        }
    );

    return;
}

void UChatMenu::ChangeChatMenuVisibilityStateBasedOnPreviewEntries(void)
{
    if (this->GetChatMenuVisibility() != EChatMenuVisibility::Preview)
    {
        return;
    }

    if (this->PreviewEntries.IsEmpty())
    {
        this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);
    }

    return;
}

UChatMenuEntry* UChatMenu::ConstructChatMenuEntry(const FChatMessage& Message) const
{
    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();
    check( SlateSettings )

    UChatMenuEntry* ChatMenuEntry = CreateWidget<UChatMenuEntry>(this->GetWorld(), SlateSettings->ChatMenuEntryWidgetClass);
    ChatMenuEntry->PassDataToWidget(FChatMessagePassData(Message));

    return ChatMenuEntry;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UChatMenu::OnChatTextChanged(const FText& Text)
{
    if (ChatStatics::IsTextToLong(Text))
    {
        this->EditableText_StdIn->SetText(FText::FromString(this->EditableText_StdIn->GetText().ToString().Left(ChatStatics::MaxChatInputLength)));
    }

    this->UpdateCmdSuggestions(Text);

    return;
}

void UChatMenu::UpdateCmdSuggestions(const FText& Text) const
{
    if (Text.IsEmpty() || CommandStatics::IsCommand(Text) == false)
    {
        this->HideCommandSuggestionsWindow();
        this->MarkCommandInAsValid();
        return;
    }

    const UShippedWorldChatCommandRegistry* CommandSubsystem = this->GetWorld()->GetSubsystem<UShippedWorldChatCommandRegistry>();
    check( CommandSubsystem )

    const FString      StdIn   = Text.ToString();
    TArray<FString>    Args;
    const FChatCommand Command = CommandStatics::GetCommandWithArgs(Text, Args);

    bool bIsClientCommand;
    bool bStdInStartWithSpecificCommandType = CommandStatics::DoesStdInStartWithSpecificCommandType(Text, bIsClientCommand);

    if (StdIn.Contains(TEXT(" ")) == false)
    {
        /* The user is still typing the command to exec. */

        const TArray<FString> RegisteredCommands = CommandSubsystem->GetCommandsThatStartWith(
            CommandStatics::GetCommand(Text),
            bStdInStartWithSpecificCommandType == false
        );

        if (RegisteredCommands.IsEmpty())
        {
            this->HideCommandSuggestionsWindow();
            return;
        }

        this->ShowCommandSuggestionsWindow(RegisteredCommands, false);

        for (const FString& RegisteredCommand : RegisteredCommands)
        {
            if (RegisteredCommand == Command)
            {
                const FString PrefixedCommand = CommandSubsystem->SmartPrefix(Command);
                if (PrefixedCommand.IsEmpty())
                {
                    this->MarkCommandInAsInvalid();
                    return;
                }

                const FChatCommandObject* CommandObj = nullptr;
                if (CommandSubsystem->GetAnyCommandObj(PrefixedCommand, CommandObj) == false)
                {
                    checkNoEntry()
                    this->MarkCommandInAsInvalid();
                    this->HideCommandSuggestionsWindow();
                    return;
                }

                if (CommandObj->Syntax.IsEmpty())
                {
                    this->MarkCommandInAsValid();
                    return;
                }

                this->MarkCommandInAsInvalid();

                return;
            }
        }

        return;
    }

    /* The user has typed a command to exec. */

    const FString PrefixedCommand = CommandSubsystem->SmartPrefix(Command);

    if (PrefixedCommand.IsEmpty())
    {
        this->MarkCommandInAsInvalid();
        this->HideCommandSuggestionsWindow();
        return;
    }

    const FChatCommandObject* CommandObj = nullptr;
    if (CommandSubsystem->GetAnyCommandObj(PrefixedCommand, CommandObj) == false)
    {
        checkNoEntry()
        this->MarkCommandInAsInvalid();
        this->HideCommandSuggestionsWindow();
        return;
    }
    check( CommandObj )

    if (CommandObj->Syntax.IsEmpty())
    {
        if (Args.IsEmpty())
        {
            this->MarkCommandInAsValid();
            this->HideCommandSuggestionsWindow();
        }
        else
        {
            this->MarkCommandInAsInvalid();
            this->ShowCommandSuggestionsWindow(*CommandObj);
        }

        return;
    }

    FString BaseSuggestion = PrefixedCommand;

    bool bCurrentArgsValid = true;

    int32      FailedSyntaxIndex = -1;
    FArgCursor MovableArgCursor  =  0;
    FArgCursor PreviousArgCursor =  0;
    bool bBadInput = false;
    for (int32 i = 0; i < CommandObj->Syntax.Num(); ++i)
    {
        const FArgCursor Temp = MovableArgCursor;
        FString NotUsed;
        if (::CommandStatics::Syntax::ParseArgument(Args, CommandObj->Syntax[i], NotUsed, MovableArgCursor, bBadInput))
        {
            PreviousArgCursor = Temp;
            continue;
        }

        if (bBadInput)
        {
            for (int32 j = 0; j < MovableArgCursor; ++j)
            {
                BaseSuggestion += TEXT(" ") + Args[j];
            }

            BaseSuggestion += TEXT(" ") + LexToString(CommandObj->Syntax[i]).ToUpper();

            this->MarkCommandInAsInvalid();
            this->ShowCommandSuggestionsWindow({BaseSuggestion});
            return;
        }

        bCurrentArgsValid = false;
        FailedSyntaxIndex = i;
        break;
    }

    if (bCurrentArgsValid)
    {
        /* More args were given than syntax wants. */
        if (Args.Num() > MovableArgCursor)
        {
            this->ShowCommandSuggestionsWindow(*CommandObj);
            this->MarkCommandInAsInvalid();

            return;
        }

        this->MarkCommandInAsValid();
        this->HideCommandSuggestionsWindow();
        return;
    }

    if (Text.ToString().EndsWith(TEXT(" ")) == false)
    {
        FailedSyntaxIndex = --FailedSyntaxIndex < 0 ? 0 : FailedSyntaxIndex;
    }

    /* Leave the last arg out as we suggest possible inputs. */
    for (int32 i = 0; i < PreviousArgCursor && Args.Num(); ++i)
    {
        BaseSuggestion += TEXT(" ") + Args[i];
    }

    TArray<FString> PossibleInputs; ::CommandStatics::Syntax::GetAllAvailableInputsForSyntax(
        *this->GetWorld(),
        CommandObj->Syntax[FailedSyntaxIndex],
        Args,
        PreviousArgCursor,
        PossibleInputs
    );

    TArray<FString> Suggestions;

    if (PossibleInputs.IsEmpty())
    {
        FString Suggestion = BaseSuggestion;
        if (Text.ToString().EndsWith(TEXT(" ")))
        {
            for (int32 i = PreviousArgCursor; i < MovableArgCursor; ++i)
            {
                Suggestion += TEXT(" ") + Args[i];
            }
        }

        Suggestions.Emplace(Suggestion + TEXT(" ") + LexToString(CommandObj->Syntax[FailedSyntaxIndex]).ToUpper());
    }
    else
    {
        for (const FString& PossibleInput : PossibleInputs)
        {
            Suggestions.Emplace(BaseSuggestion + TEXT(" ") + PossibleInput);
        }

        Suggestions.Emplace(BaseSuggestion + TEXT(" ") + FString::Join(Args, TEXT(" ")));
    }

    this->MarkCommandInAsInvalid();
    this->ShowCommandSuggestionsWindow(Suggestions);

    return;
}

void UChatMenu::MarkCommandInAsInvalid(void) const
{
    this->Border_StdInWrapper->SetTemporarilyColor(FColor::Red, true);
}

void UChatMenu::MarkCommandInAsValid(void) const
{
    this->Border_StdInWrapper->UpdateComponentWithTheirScheme();
}

void UChatMenu::HideCommandSuggestionsWindow(void) const
{
    if (this->VerticalBox_CmdSuggestions->GetVisibility() != ESlateVisibility::Collapsed)
    {
        this->VerticalBox_CmdSuggestions->SetVisibility(ESlateVisibility::Collapsed);
    }

    this->VerticalBox_CmdSuggestions->ClearChildren();

    return;
}

void UChatMenu::ShowCommandSuggestionsWindow(const TArray<FString>& Content, const bool bUpdateStdInValidityFeedback /* = false */) const
{
    if (Content.IsEmpty())
    {
        this->HideCommandSuggestionsWindow();
        return;
    }

    if (this->VerticalBox_CmdSuggestions->GetVisibility() != ESlateVisibility::Visible)
    {
        this->VerticalBox_CmdSuggestions->SetVisibility(ESlateVisibility::Visible);
    }

    this->VerticalBox_CmdSuggestions->ClearChildren();

    bool          bCommandValid = false;
    const FString Command       = CommandStatics::GetCommand(this->EditableText_StdIn->GetText());

    for (const FString& ContentCommand : Content)
    {
        UJAFGTextBlock* TextBlock = WidgetTree->ConstructWidget<UJAFGTextBlock>(UJAFGTextBlock::StaticClass());
        TextBlock->SetText(FText::FromString(ContentCommand));
        TextBlock->SetColorScheme(EJAFGFontSize::Small);
        TextBlock->UpdateComponentWithTheirScheme();

        this->VerticalBox_CmdSuggestions->AddChild(TextBlock);

        if (bUpdateStdInValidityFeedback && ContentCommand == Command)
        {
            bCommandValid = true;
        }

        continue;
    }

    if (bCommandValid)
    {
        this->MarkCommandInAsValid();
    }
    else
    {
        this->MarkCommandInAsInvalid();
    }

    return;
}

void UChatMenu::ShowCommandSuggestionsWindow(const FChatCommandObject& InObj) const
{
    FString FullCommandRepresentation = InObj.Command;
    for (const EChatCommandSyntax::Type& Syntax : InObj.Syntax)
    {
        FullCommandRepresentation += TEXT(" ") + LexToString(Syntax);
    }

    this->ShowCommandSuggestionsWindow({ FullCommandRepresentation }, false);

    return;
}

void UChatMenu::OnChatTextCommitted(const FText& Text, const ETextCommit::Type CommitMethod)
{
    /* Ignoring commits that are not passed through the key Enter. */
    if (
           CommitMethod == ETextCommit::Default
        || CommitMethod == ETextCommit::OnUserMovedFocus
        /*
         * Kinda sketchy here. But we let the enhanced input subsystem handle escape key presses.
         * Therefore, we would get this request here twice. This is unwanted.
         */
        || CommitMethod == ETextCommit::OnCleared
    )
    {
        return;
    }

    if (this->IsStdInValid() == false)
    {
        if (Text.ToString().IsEmpty())
        {
            this->HideChatMenu();
            return;
        }

        LOG_WARNING(
            LogJAFGChat,
            "Chat text not committed. Text [%s]; Method: [%d]. Not valid.",
            *Text.ToString(), static_cast<int32>(CommitMethod)
        )

        return;
    }

    this->HideChatMenu();

    OWNING_CHAT_COMPONENT->ParseMessage(Text);

    return;
}

FOnKeyDown UChatMenu::GetOnStdInKeyDownHandler(void)
{
    return FOnKeyDown::CreateLambda(
        [this] (const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) -> FReply
        {
            const UJAFGInputSubsystem* InputSubsystem = this->GetOwningLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();

            if (InputSubsystem->GetAllKeysForAction(InputActions::PreviousChatStdIn).Contains(InKeyEvent.GetKey()))
            {
                this->OnHistoryLookUp(true);
                return FReply::Handled();
            }

            if (InputSubsystem->GetAllKeysForAction(InputActions::NextChatStdIn).Contains(InKeyEvent.GetKey()))
            {
                this->OnHistoryLookUp(false);
                return FReply::Handled();
            }

            return FReply::Unhandled();
        }
    );
}

void UChatMenu::OnHistoryLookUp(const bool bPrevious)
{
    this->CurrentCursorInHistory = bPrevious ? this->CurrentCursorInHistory + 1 : this->CurrentCursorInHistory - 1;

    if (this->CurrentCursorInHistory < 0)
    {
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
        this->EditableText_StdIn->SetText(FText::GetEmpty());
        return;
    }
    if (OWNING_CHAT_COMPONENT->ChatStdInLog.Num() <= this->CurrentCursorInHistory)
    {
        this->CurrentCursorInHistory = OWNING_CHAT_COMPONENT->ChatStdInLog.Num() - 1;
        return;
    }

#if WITH_EDITOR
    if (OWNING_CHAT_COMPONENT->ChatStdInLog[this->CurrentCursorInHistory].IsEmpty())
    {
        LOG_WARNING(LogJAFGChat, "Chat stdin log at index %d is empty.", this->CurrentCursorInHistory)
    }
#endif /* WITH_EDITOR */

    this->EditableText_StdIn->SetText(OWNING_CHAT_COMPONENT->ChatStdInLog[this->CurrentCursorInHistory]);

    return;
}

void UChatMenu::FocusStdIn(void) const
{
    this->EditableText_StdIn->SetUserFocus(OWNING_PLAYER_CONTROLLER);
}

auto UChatMenu::ClearStdIn(void) const -> void
{
    this->EditableText_StdIn->SetText(FText::GetEmpty());
}

bool UChatMenu::IsStdInValid(void) const
{
   return ChatStatics::IsTextValid(this->EditableText_StdIn->GetText());
}

#undef OWNING_PLAYER_CONTROLLER
#undef OWNING_CHAT_COMPONENT
