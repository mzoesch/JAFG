// Copyright 2024 mzoesch. All rights reserved.

#include "ChatMenu.h"

#include "ChatComponent.h"
#include "CommonChatStatics.h"
#include "EnhancedInputSubsystems.h"
#include "JAFGSlateSettings.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Input/CustomInputNames.h"
#include "Input/JAFGEditableTextBlock.h"
#include "Input/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
#include "JAFGLogDefs.h"

#define OWNING_PLAYER_CONTROLLER                             \
    Cast<AWorldPlayerController>(this->GetOwningPlayer())
#define OWNING_CHAT_COMPONENT                                \
    Cast<UChatComponent>(                                    \
        Cast<AWorldPlayerController>(this->GetOwningPlayer() \
    )->GetComponentByClass(UChatComponent::StaticClass()))

void UChatMenuEntry::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FChatMessageData* ChatMessageData = static_cast<const FChatMessageData*>(&MyPassedData); ChatMessageData == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogJAFGChat, "MyPassedData is not of type FChatMessageData.")
#else
        LOG_FATAL(LogJAFGChat, "MyPassedData is not of type FChatMessageData.")
#endif /* WITH_EDITOR */
    }
    else
    {
        this->Data = *ChatMessageData;
    }

    this->ConstructMessage();

    this->OnDeferredConstruct();

    return;
}

void UChatMenuEntry::ConstructMessage(void)
{
    this->TB_Message->SetText(
        FText::FromString(
            FString::Printf(
                TEXT("<%s> %s"),
                *this->Data.Sender, *this->Data.Message.ToString()
            )
        )
    );

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
#if WITH_EDITOR
        LOG_ERROR(LogJAFGChat, "Chat component is not attached to the owning player controller.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogJAFGChat, "Chat component is not attached to the owning player controller.")
#endif /* !WITH_EDITOR */
    }

    this->PreviewEntries.Empty();

    this->ChatMenuVisibilityChangedHandle = Owner->SubscribeToChatVisibilityChanged(ADD_SLATE_VIS_DELG(UChatMenu::OnChatMenuVisibilityChanged));
    this->ChatHistoryLookupHandle         = Owner->SubscribeToChatHistoryLookup(FChatHistoryLookupSignature::FDelegate::CreateUObject(this, &UChatMenu::OnHistoryLookUp));

    this->ET_StdIn->SetCustomEventToKeyDown(this->GetOnStdInKeyDownHandler());
    this->ET_StdIn->OnTextChanged.AddDynamic(this, &UChatMenu::OnChatTextChanged);
    this->ET_StdIn->OnTextCommitted.AddDynamic(this, &UChatMenu::OnChatTextCommitted);

    this->ChangeChatMenuVisibility(EChatMenuVisibility::Collapsed);

    /* For messages that have arrived while this widget has not yet been constructed. */
    while (OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.IsEmpty() == false)
    {
        FPrivateMessagePreConstruct PreConstruct;
        OWNING_CHAT_COMPONENT->PreChatWidgetConstructionQueue.Dequeue(PreConstruct);

        this->AddMessageToChatLog(PreConstruct.Sender, PreConstruct.Message);
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

void UChatMenu::AddMessageToChatLog(const FString& Sender, const FText& Message)
{
    this->SB_StdOut->AddChild(this->ConstructChatMenuEntry(Sender, Message));
    this->SafeAddToPreviewOut(Sender, Message);

    if (this->GetChatMenuVisibility() == EChatMenuVisibility::Full)
    {
        if (this->SB_StdOut->GetScrollOffsetOfEnd() - this->SB_StdOut->GetScrollOffset() < this->StdOutScrollOffsetOfEndIgnoreDelta)
        {
            /** Give the scroll bar time to render. And exectue on the next tick. */
            AsyncTask(ENamedThreads::GameThread, [this] (void) -> void
            {
                this->SB_StdOut->ScrollToEnd();
            });
        }
        return;
    }

    this->ChangeChatMenuVisibility(EChatMenuVisibility::Preview);

    return;
}

void UChatMenu::ClearAllChatEntries(void)
{
    this->SB_StdOut->ClearChildren();
    this->VB_PreviewOut->ClearChildren();
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
    Subsystem->AddMappingContext(JAFGSubsystem->GetContextByName(bVisible ? InputContexts::Chat : InputContexts::Foot), 0);

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
        this->VB_PreviewOut->ClearChildren();
        this->PreviewEntries.Empty();
        this->ClearStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
        break;
    }
    case EChatMenuVisibility::Preview:
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->PW_StdOutWrapper->SetVisibility(ESlateVisibility::Collapsed);
        this->PW_PreviewOutWrapper->SetVisibility(ESlateVisibility::Visible);
        this->PW_StdInWrapper->SetVisibility(ESlateVisibility::Hidden);

        this->ClearStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;

        break;
    }
    case EChatMenuVisibility::Full:
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->PW_StdOutWrapper->SetVisibility(ESlateVisibility::Visible);
        this->PW_PreviewOutWrapper->SetVisibility(ESlateVisibility::Collapsed);
        this->PW_StdInWrapper->SetVisibility(ESlateVisibility::Visible);

        this->SB_StdOut->ScrollToEnd();
        this->ClearStdIn();
        this->FocusStdIn();
        this->CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;

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

    if (this->PW_PreviewOutWrapper->GetVisibility() == ESlateVisibility::Visible)
    {
        return EChatMenuVisibility::Preview;
    }

    return EChatMenuVisibility::Full;
}

void UChatMenu::SafeAddToPreviewOut(const FString& Sender, const FText& Message)
{
    UUserWidget* Entry = this->ConstructChatMenuEntry(Sender, Message);
    this->VB_PreviewOut->AddChild(Entry);

    FPrivatePreviewEntryData Data;
    Data.CreationTimeInSeconds = this->GetWorld()->GetTimeSeconds();
    Data.Entry                 = Entry;

    this->PreviewEntries.Emplace(Data);

    while (this->VB_PreviewOut->GetChildrenCount() > this->MaxEntriesInPreviewOut)
    {
        UUserWidget* EntryToRemove = Cast<UUserWidget>(this->VB_PreviewOut->GetChildAt(0));
        this->PreviewEntries.RemoveAll(
            [EntryToRemove] (const FPrivatePreviewEntryData& ExistingData) -> bool
            {
                return ExistingData.Entry == EntryToRemove;
            }
        );
        this->VB_PreviewOut->RemoveChildAt(0);
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

                this->VB_PreviewOut->RemoveChild(Data.Entry);
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

UChatMenuEntry* UChatMenu::ConstructChatMenuEntry(const FString& Sender, const FText& Message) const
{
    FChatMessageData ChatMessageData = FChatMessageData();
    ChatMessageData.Sender  = Sender;
    ChatMessageData.Message = Message;

    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();
    check( SlateSettings )

    UChatMenuEntry* ChatMenuEntry = CreateWidget<UChatMenuEntry>(this->GetWorld(), SlateSettings->ChatMenuEntryWidgetClass);
    ChatMenuEntry->PassDataToWidget(ChatMessageData);

    return ChatMenuEntry;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UChatMenu::OnChatTextChanged(const FText& Text)
{
    if (ChatStatics::IsTextToLong(Text))
    {
        this->ET_StdIn->SetText(FText::FromString(this->ET_StdIn->GetText().ToString().Left(ChatStatics::MaxChatInputLength)));
    }

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
        this->ET_StdIn->SetText(FText::GetEmpty());
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

    this->ET_StdIn->SetText(OWNING_CHAT_COMPONENT->ChatStdInLog[this->CurrentCursorInHistory]);

    return;
}

void UChatMenu::FocusStdIn(void) const
{
    this->ET_StdIn->SetUserFocus(OWNING_PLAYER_CONTROLLER);
}

auto UChatMenu::ClearStdIn(void) const -> void
{
    this->ET_StdIn->SetText(FText::GetEmpty());
}

bool UChatMenu::IsStdInValid(void) const
{
   return ChatStatics::IsTextValid(this->ET_StdIn->GetText());
}

#undef OWNING_PLAYER_CONTROLLER
#undef OWNING_CHAT_COMPONENT
