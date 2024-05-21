// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/JoinSessionPanelWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "UI/FrontEnd/RemoteSessionEntry.h"

void UJoinSessionPanelWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    check( this->RemoteSessionEntryWidgetClass )
    check( this->NoRemoteSessionsFoundPlaceholderWidgetClass )
    check( this->WaitingForSearchPlaceholderWidgetClass )

    if (this->B_Search)
    {
        this->B_Search->OnClicked.AddDynamic(this, &UJoinSessionPanelWidget::RefreshRemoteSessions);
    }

    this->SetPlaceholderToNoRemoteSessionsFound();

    return;
}

void UJoinSessionPanelWidget::OnNativeMadeVisible(void)
{
    Super::OnNativeMadeVisible();

    this->RefreshRemoteSessions();

    return;
}

void UJoinSessionPanelWidget::OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem)
{
    LOG_WARNING(LogLSSSS, "Called.")

    if (this->B_Search)
    {
        this->B_Search->SetIsEnabled(true);
    }

    if (bSuccess == false)
    {
        this->SetPlaceholderToNoRemoteSessionsFound();
        return;
    }

    if (Subsystem->GetSearch().Search.Get().SearchResults.IsEmpty())
    {
        this->SetPlaceholderToNoRemoteSessionsFound();
        return;
    }

    this->SB_RemoteSessions->ClearChildren();
    for (int i = 0; i < Subsystem->GetSearch().Search.Get().SearchResults.Num(); ++i)
    {
        const FOnlineSessionSearchResult& SearchResult = Subsystem->GetSearch().Search.Get().SearchResults[i];

        FPassedRemoteSessionEntryData Data = FPassedRemoteSessionEntryData();

        Data.WidgetIdentifier        = i;
        Data.OnWidgetPressedDelegate = [this] (const int32 WidgetIdentifier) { this->OnNativeRemoteSessionEntryClicked(WidgetIdentifier); };

        Data.IDStr = SearchResult.GetSessionIdStr();
        Data.PingInMs = SearchResult.PingInMs;

        Data.OwningUserID = SearchResult.Session.OwningUserId->ToString();
        Data.OwningUserName = SearchResult.Session.OwningUserName;
        Data.NumOpenPrivateConnections = SearchResult.Session.NumOpenPrivateConnections;
        Data.NumOpenPublicConnections = SearchResult.Session.NumOpenPublicConnections;

        Data.NumPublicConnections = SearchResult.Session.SessionSettings.NumPublicConnections;
        Data.NumPrivateConnections = SearchResult.Session.SessionSettings.NumPrivateConnections;
        Data.bLANSession = SearchResult.Session.SessionSettings.bIsLANMatch;
        Data.bDedicatedSession = SearchResult.Session.SessionSettings.bIsDedicated;

        URemoteSessionEntry* RemoteSessionEntry = CreateWidget<URemoteSessionEntry>(this, this->RemoteSessionEntryWidgetClass);
        RemoteSessionEntry->PassDataToWidget(Data);

        this->SB_RemoteSessions->AddChild(RemoteSessionEntry);
    }

    return;
}

void UJoinSessionPanelWidget::OnNativeRemoteSessionEntryClicked(const int32 WidgetIdentifier)
{
    if (this->HasFocusedRemoteSessionEntry())
    {
        Cast<UJAFGFocusableWidget>(this->SB_RemoteSessions->GetChildAt(this->CurrentlyFocusedRemoteSessionEntryIndex))->SetWidgetUnfocus();
    }

    if (this->CurrentlyFocusedRemoteSessionEntryIndex == WidgetIdentifier)
    {
        this->CurrentlyFocusedRemoteSessionEntryIndex = this->InvalidFocusedRemoteSessionEntryIndex;
        return;
    }

    this->CurrentlyFocusedRemoteSessionEntryIndex = WidgetIdentifier;
    Cast<UJAFGFocusableWidget>(this->SB_RemoteSessions->GetChildAt(this->CurrentlyFocusedRemoteSessionEntryIndex))->SetWidgetFocus();

    return;
}

void UJoinSessionPanelWidget::RefreshRemoteSessions(void)
{
    if (this->B_Search)
    {
        this->B_Search->SetIsEnabled(false);
    }

    ULocalSessionSupervisorSubsystem* LocalSessionSupervisorSubsystem = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();

    LocalSessionSupervisorSubsystem->FindSafeSessions(32, true, this);
    this->SetPlaceholderToWaitingForSearch();

    return;
}

void UJoinSessionPanelWidget::JoinSession(void) const
{
    if (this->CurrentlyFocusedRemoteSessionEntryIndex == this->InvalidFocusedRemoteSessionEntryIndex)
    {
        LOG_FATAL(LogCommonSlate, "No remote session is focused.")
        return;
    }

    if (
        const ULocalSessionSupervisorSubsystem* LSSSS = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
        LSSSS->GetSearch().Search.Get().SearchResults.IsValidIndex(this->CurrentlyFocusedRemoteSessionEntryIndex) == false
    )
    {
        LOG_FATAL(LogCommonSlate, "Invalid remote session index.")
        return;
    }

    return;
}

void UJoinSessionPanelWidget::SetPlaceholderToNoRemoteSessionsFound(void)
{
    this->SB_RemoteSessions->ClearChildren();
    this->SB_RemoteSessions->AddChild(CreateWidget<UUserWidget>(this, this->NoRemoteSessionsFoundPlaceholderWidgetClass));

    return;
}

void UJoinSessionPanelWidget::SetPlaceholderToWaitingForSearch(void)
{
    this->SB_RemoteSessions->ClearChildren();
    this->SB_RemoteSessions->AddChild(CreateWidget<UUserWidget>(this, this->WaitingForSearchPlaceholderWidgetClass));

    return;
}
