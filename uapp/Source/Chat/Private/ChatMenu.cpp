// Copyright 2024 mzoesch. All rights reserved.

#include "ChatMenu.h"
#include "ChatComponentImpl.h"
#include "ChatStatics.h"
#include "DefaultColorsSubsystem.h"
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

#ifdef SPACE
    #error "SPACE macro is already defined."
#endif /* SPACE */
#define SPACE                                                  \
    TEXT(" ")
#define OWNING_PLAYER_CONTROLLER                               \
    Cast<AWorldPlayerController>(this->GetOwningPlayer())
#define OWNING_CHAT_COMPONENT                                  \
    Cast<UChatComponentImpl>(                                  \
        Cast<AWorldPlayerController>(this->GetOwningPlayer()   \
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

    this->ChatMenuVisibilityChangedHandle        =
        Owner->SubscribeToChatVisibilityChanged(ADD_SLATE_VIS_DELG(UChatMenu::OnChatMenuVisibilityChanged));
    this->ChatHistoryLookupHandle                =
        Owner->SubscribeToChatHistoryLookup(FChatHistoryLookupSignature::FDelegate::CreateUObject(this, &UChatMenu::OnHistoryLookUp));
    this->FillCommandSuggestionToChatStdInHandle =
        Owner->SubscribeToFillSuggestionToChatStdIn(FOnFillSuggestionToChatStdInDelegateSignature::FDelegate::CreateUObject(this, &UChatMenu::OnFillCommandSuggestion));

    this->EditableText_StdIn->SetCustomEventToKeyDown(this->GetOnStdInKeyDownHandler());
    this->EditableText_StdIn->SetMaxSize(ChatStatics::MaxChatInputLength);
    const TFunction<bool(const FText& InText)> OnTrimmedTextChangedB = [this] (const FText& InText) -> bool { return CommandStatics::IsCommand(InText); };
    this->EditableText_StdIn->SetMaxSizeB(CommandStatics::MaxChatInputLength, &OnTrimmedTextChangedB);
    this->EditableText_StdIn->OnTrimmedTextChanged.AddLambda( [this] (const FText& InText) -> void { this->OnChatTextChanged(InText); });
    this->EditableText_StdIn->OnTextCommitted.AddDynamic(this, &UChatMenu::OnChatTextCommitted);

    this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);

    /* For messages that have arrived while this widget has not yet been constructed. */
    while (OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.IsEmpty() == false)
    {
        FChatMessage PreConstruct;
        OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.Dequeue(PreConstruct);

        this->AddMessageToChatLog(PreConstruct);
    }

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

    if (OWNING_PLAYER_CONTROLLER->UnSubscribeToFillSuggestionToChatStdIn(this->FillCommandSuggestionToChatStdInHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Fill Command Suggestion To Chat StdIn event.")
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
        this->MarkCommandInAsValid();
        this->CommandSuggestions.Empty();
        this->CurrentCursorInCmdSuggestions = UChatMenu::InvalidCursorInCmdSuggestions;
        this->EditableText_StdIn->ResetLastValidText();
        this->LastSelfTypedIn = FText::GetEmpty();
    }
    else
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Chat is now hidden.")
        this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);
        this->MarkCommandInAsValid();
        this->CommandSuggestions.Empty();
        this->CurrentCursorInCmdSuggestions = UChatMenu::InvalidCursorInCmdSuggestions;
        this->EditableText_StdIn->ResetLastValidText();
        this->LastSelfTypedIn = FText::GetEmpty();
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

void UChatMenu::UpdateCmdSuggestions(const FText& InText) const
{
    if (InText.IsEmpty() || CommandStatics::IsCommand(InText) == false)
    {
        this->HideCommandSuggestionsWindow();
        this->MarkCommandInAsValid();
        return;
    }

    const UShippedWorldChatCommandRegistry* CommandSubsystem = this->GetWorld()->GetSubsystem<UShippedWorldChatCommandRegistry>();
    check( CommandSubsystem )

    const FString      StdIn               = InText.ToString();
    const bool         bStdInTrailingSpace = StdIn.EndsWith(SPACE);
    TArray<FString>    Args                = TArray<FString>();
    const FChatCommand Command             = CommandStatics::GetCommandWithArgs(InText, Args);

          bool bIsClientCommand;
    const bool bStdInStartWithSpecificCommandType = CommandStatics::DoesStdInStartWithSpecificCommandType(InText, bIsClientCommand);

    if (StdIn.Contains(SPACE) == false)
    {
        /* The user is still typing the command to exec. */

        TArray<FString> RegisteredCommands = CommandSubsystem->GetCommandsThatStartWith(
            CommandStatics::GetCommand(InText),
            bStdInStartWithSpecificCommandType == false
        );

        if (RegisteredCommands.IsEmpty())
        {
            this->HideCommandSuggestionsWindow();
            return;
        }

        RegisteredCommands.Sort( [] (const FString& A, const FString& B) { return A > B; });

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

        this->MarkCommandInAsInvalid();

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

    FString    BaseSuggestion    = PrefixedCommand;

    bool       bCurrentArgsValid = true;

    int32      FailedSyntaxIndex = -1;
    FArgCursor MovableArgCursor  =  0;
    FArgCursor PreviousArgCursor =  0;
    bool       bBadInput         = false;
    for (int32 i = 0; i < CommandObj->Syntax.Num(); ++i)
    {
        const FArgCursor Temp = MovableArgCursor;
        if (FString NotUsed; ::CommandStatics::Syntax::ParseArgument(
            *this->GetWorld(),
            Args,
            CommandObj->Syntax[i],
            NotUsed,
            MovableArgCursor,
            bBadInput
        ))
        {
            PreviousArgCursor = Temp;
            continue;
        }

        if (bBadInput)
        {
            for (int32 j = 0; j < MovableArgCursor; ++j)
            {
                BaseSuggestion += SPACE + Args[j];
            }

            BaseSuggestion += SPACE + LexToString(CommandObj->Syntax[i]).ToUpper();

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

    if (bStdInTrailingSpace == false)
    {
        FailedSyntaxIndex = --FailedSyntaxIndex < 0 ? 0 : FailedSyntaxIndex;
    }

    /* Leave the last arg out as we suggest possible inputs. */
    for (int32 i = 0; i < PreviousArgCursor && Args.Num(); ++i)
    {
        BaseSuggestion += SPACE + Args[i];
    }

    if (bStdInTrailingSpace)
    {
        if (Args.IsValidIndex(PreviousArgCursor))
        {
            BaseSuggestion += SPACE + Args[PreviousArgCursor];
        }
    }

    TArray<FString> PossibleInputs; ::CommandStatics::Syntax::GetAllAvailableInputsForSyntax(
        *this->GetWorld(),
        CommandObj->Syntax[FailedSyntaxIndex],
        Args,
        bStdInTrailingSpace ? MovableArgCursor : PreviousArgCursor,
        PossibleInputs
    );

    TArray<FString> Suggestions;

    if (PossibleInputs.IsEmpty())
    {
        Suggestions.Emplace(BaseSuggestion + SPACE + LexToString(CommandObj->Syntax[FailedSyntaxIndex]).ToUpper());
    }
    else
    {
        for (const FString& PossibleInput : PossibleInputs)
        {
            Suggestions.Emplace(BaseSuggestion + SPACE + PossibleInput);
        }
    }

    this->MarkCommandInAsInvalid();
    this->ShowCommandSuggestionsWindow(Suggestions);

    return;
}

void UChatMenu::UpdateCmdSuggestions(void) const
{
    this->UpdateCmdSuggestions(this->EditableText_StdIn->GetText());
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
        this->CommandSuggestions.Empty();
        this->HideCommandSuggestionsWindow();
        return;
    }

    this->CommandSuggestions = Content;

    if (this->VerticalBox_CmdSuggestions->GetVisibility() != ESlateVisibility::Visible)
    {
        this->VerticalBox_CmdSuggestions->SetVisibility(ESlateVisibility::Visible);
    }

    this->VerticalBox_CmdSuggestions->ClearChildren();

    bool          bCommandValid = false;
    const FString Command       = CommandStatics::GetCommand(this->EditableText_StdIn->GetText());

    for (int32 i = this->CommandSuggestions.Num() - 1; i >= 0; --i)
    {
        UJAFGBorder* Wrapper = WidgetTree->ConstructWidget<UJAFGBorder>(UJAFGBorder::StaticClass());
        Wrapper->SetTemporarilyColor(i == this->CurrentCursorInCmdSuggestions ? FColor::Black : FColor::Transparent, false);

        UJAFGTextBlock* TextBlock = WidgetTree->ConstructWidget<UJAFGTextBlock>(UJAFGTextBlock::StaticClass());
        TextBlock->SetText(FText::FromString(this->CommandSuggestions[i]));
        TextBlock->SetColorScheme(EJAFGFontSize::Small);
        TextBlock->UpdateComponentWithTheirScheme();
        Wrapper->AddChild(TextBlock);

        this->VerticalBox_CmdSuggestions->AddChild(Wrapper);

        if (bUpdateStdInValidityFeedback && this->CommandSuggestions[i] == Command)
        {
            bCommandValid = true;
        }

        continue;
    }

    if (bUpdateStdInValidityFeedback)
    {
        if (bCommandValid)
        {
            this->MarkCommandInAsValid();
        }
        else
        {
            this->MarkCommandInAsInvalid();
        }
    }

    return;
}

void UChatMenu::ShowCommandSuggestionsWindow(const FChatCommandObject& InObj) const
{
    FString FullCommandRepresentation = InObj.Command;
    for (const EChatCommandSyntax::Type& Syntax : InObj.Syntax)
    {
        FullCommandRepresentation += SPACE + LexToString(Syntax);
    }

    this->ShowCommandSuggestionsWindow({ FullCommandRepresentation }, false);

    return;
}

void UChatMenu::RerenderCommandSuggestionsWindow(void) const
{
    if (this->CommandSuggestions.IsEmpty())
    {
        jrelaxedCheckNoEntry()
        return;
    }

    if (this->VerticalBox_CmdSuggestions->GetVisibility() != ESlateVisibility::Visible)
    {
        this->VerticalBox_CmdSuggestions->SetVisibility(ESlateVisibility::Visible);
    }

    this->VerticalBox_CmdSuggestions->ClearChildren();

    for (int32 i = this->CommandSuggestions.Num() - 1; i >= 0; --i)
    {
        UJAFGBorder* Wrapper = WidgetTree->ConstructWidget<UJAFGBorder>(UJAFGBorder::StaticClass());
        Wrapper->SetTemporarilyColor(i == this->CurrentCursorInCmdSuggestions ? FColor::Black : FColor::Transparent, false);

        UJAFGTextBlock* TextBlock = WidgetTree->ConstructWidget<UJAFGTextBlock>(UJAFGTextBlock::StaticClass());
        TextBlock->SetText(FText::FromString(this->CommandSuggestions[i]));
        TextBlock->SetColorScheme(EJAFGFontSize::Small);
        TextBlock->UpdateComponentWithTheirScheme();
        Wrapper->AddChild(TextBlock);

        this->VerticalBox_CmdSuggestions->AddChild(Wrapper);

        continue;
    }

    return;
}

void UChatMenu::OnChatTextCommitted(const FText& InText, const ETextCommit::Type CommitMethod)
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
        if (InText.ToString().IsEmpty())
        {
            this->HideChatMenu();
            return;
        }

        LOG_WARNING(
            LogJAFGChat,
            "Chat text not committed. Text [%s]; Method: [%d]. Not valid.",
            *InText.ToString(), static_cast<int32>(CommitMethod)
        )

        return;
    }

    this->HideChatMenu();

    OWNING_CHAT_COMPONENT->ParseMessage(InText);

    return;
}

void UChatMenu::OnChatTextChanged(const FText& InText)
{
    this->LastSelfTypedIn        = InText;
    this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;

    this->UpdateCmdSuggestions(InText);

    return;
}

FOnKeyDown UChatMenu::GetOnStdInKeyDownHandler(void)
{
    return FOnKeyDown::CreateLambda(
        [this] (const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) -> FReply
        {
            const UJAFGInputSubsystem* InputSubsystem = this->GetOwningLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();

            if (InputSubsystem->GetAllKeysForAction(InputActions::FillSuggestionToChatStdIn).Contains(InKeyEvent.GetKey()))
            {
                this->OnFillCommandSuggestion();
                return FReply::Handled();
            }

            if (InputSubsystem->GetAllKeysForAction(InputActions::PreviousChatStdIn).Contains(InKeyEvent.GetKey()))
            {
                if (
                       this->VerticalBox_CmdSuggestions->GetVisibility() == ESlateVisibility::Collapsed
                    || this->VerticalBox_CmdSuggestions->GetChildrenCount() <= 1
                )
                {
                    this->OnHistoryLookUp(true);
                }
                else
                {
                    this->OnCommandSuggestionLookUp(true);
                }

                return FReply::Handled();
            }

            if (InputSubsystem->GetAllKeysForAction(InputActions::NextChatStdIn).Contains(InKeyEvent.GetKey()))
            {
                if (
                       this->VerticalBox_CmdSuggestions->GetVisibility() == ESlateVisibility::Collapsed
                    || this->VerticalBox_CmdSuggestions->GetChildrenCount() <= 1
                )
                {
                    this->OnHistoryLookUp(false);
                }
                else
                {
                    this->OnCommandSuggestionLookUp(false);
                }

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
        this->EditableText_StdIn->SetText(this->LastSelfTypedIn);
        this->UpdateCmdSuggestions();
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
    this->UpdateCmdSuggestions();

    return;
}

void UChatMenu::OnCommandSuggestionLookUp(const bool bPrevious) const
{
    this->CurrentCursorInCmdSuggestions = bPrevious ? this->CurrentCursorInCmdSuggestions + 1 : this->CurrentCursorInCmdSuggestions - 1;

    if (this->CurrentCursorInCmdSuggestions < 0)
    {
        this->CurrentCursorInCmdSuggestions = UChatMenu::InvalidCursorInCmdSuggestions;
    }

    else if (this->CommandSuggestions.Num() <= this->CurrentCursorInCmdSuggestions)
    {
        this->CurrentCursorInCmdSuggestions = this->CommandSuggestions.Num() - 1;
    }

    this->RerenderCommandSuggestionsWindow();

    return;
}

void UChatMenu::OnFillCommandSuggestion(void) const
{
    if (this->CommandSuggestions.IsValidIndex(this->CurrentCursorInCmdSuggestions) == false)
    {
        return;
    }

    this->EditableText_StdIn->SetText(FText::FromString(FString::Printf(TEXT("%s%s"),
        *CommandStatics::CommandPrefix,
        *this->CommandSuggestions[this->CurrentCursorInCmdSuggestions]
    )));
    this->LastSelfTypedIn = this->EditableText_StdIn->GetText();

    this->UpdateCmdSuggestions(this->EditableText_StdIn->GetText());

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
    if (CommandStatics::IsCommand(this->EditableText_StdIn->GetText()))
    {
        return this->EditableText_StdIn->GetText().ToString().Len() <= CommandStatics::MaxChatInputLength;
    }

   return ChatStatics::IsTextValid(this->EditableText_StdIn->GetText());
}

#undef SPACE
#undef OWNING_PLAYER_CONTROLLER
#undef OWNING_CHAT_COMPONENT
