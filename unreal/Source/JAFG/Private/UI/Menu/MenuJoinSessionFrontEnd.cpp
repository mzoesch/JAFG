// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/MenuJoinSessionFrontEnd.h"

#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "UI/Menu/OnlineSessionEntry.h"

#if WITH_EDITOR
void UMenuJoinSessionFrontEnd::MakeMockSessionEntryData(const int32 Index, FOnlineSessionEntryData& OutSessionEntryData)
{
    OutSessionEntryData.EntryIndex = Index;

    /* We do not want to have zeros in the out data. */
    const int32 MyIndex = Index + 1 - UMenuJoinSessionFrontEnd::UsedSBIndices;
    
    OutSessionEntryData.IDStr = FString::Printf(TEXT("MockSessionEntryData_%d"), MyIndex);
    OutSessionEntryData.PingInMs = 0xA * MyIndex;
    
    OutSessionEntryData.OwningUserID = FString::Printf(TEXT("GUID-%d-%s"), /* For Debugging! */ Index, *FGuid::NewGuid().ToString());
    OutSessionEntryData.OwningUserName = OutSessionEntryData.OwningUserID;
    OutSessionEntryData.NumOpenPrivateConnections = 0xB * MyIndex;
    OutSessionEntryData.NumOpenPublicConnections = 0xC * MyIndex;
    
    OutSessionEntryData.MaxPrivateConnections = 0xE * MyIndex + (MyIndex % 2) * 0x2;
    OutSessionEntryData.MaxPublicConnections = 0xD * MyIndex + (MyIndex % 2) * 0x2;
    OutSessionEntryData.bLANSession = (MyIndex % 3) == 0;
    OutSessionEntryData.bDedicatedSession = (MyIndex % 4) == 0;

    return;
}
#endif /* WITH_EDITOR */

void UMenuJoinSessionFrontEnd::NativeConstruct()
{
    Super::NativeConstruct();

    this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Visible);
    this->O_LoadingSessionsPlaceholder->SetVisibility(ESlateVisibility::Collapsed);

    return;
}

void UMenuJoinSessionFrontEnd::JoinSession(const int32 EntryIndex) const
{
    if (this->SB_FoundSessions->GetChildrenCount() <= EntryIndex)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMenuJoinSessionFrontEnd::JoinSession: Entry index %d is out of range."), EntryIndex)
        return;
    }
    
    const UOnlineSessionEntry* Entry = CastChecked<UOnlineSessionEntry>(this->SB_FoundSessions->GetChildAt(EntryIndex));

    if (Entry == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMenuJoinSessionFrontEnd::JoinSession: Entry at index %d is invalid."), EntryIndex)
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("UMenuJoinSessionFrontEnd::JoinSession: Joining session %s."), *Entry->GetEntryData().GetFullSessionName())
    
    return;
}

void UMenuJoinSessionFrontEnd::OnOnlineSessionFoundCompleteDelegate(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem)
{
    for (UWidget* Child : this->SB_FoundSessions->GetAllChildren())
    {
        if (Child == nullptr)
        {
            continue;
        }

        if (Child->IsA<UOnlineSessionEntry>())
        {
            this->SB_FoundSessions->RemoveChild(Child);
        }
        
        continue;
    }

    this->O_LoadingSessionsPlaceholder->SetVisibility(ESlateVisibility::Collapsed);
    
    if (Subsystem->GetActiveOnlineSessionSearch()->SearchResults.Num() <= 0)
    {
        UE_LOG(LogTemp, Log, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: No sessions found."))
        this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Visible);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: Found %d sessions."), Subsystem->GetActiveOnlineSessionSearch()->SearchResults.Num())

    check( this->WB_OnlineSessionEntryClass )
    
    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: World is invalid."))
        return;
    }
    
    this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Collapsed);

    for (int32 i = UMenuJoinSessionFrontEnd::UsedSBIndices; i < Subsystem->GetActiveOnlineSessionSearch()->SearchResults.Num() + UMenuJoinSessionFrontEnd::UsedSBIndices; ++i)
    {
        FOnlineSessionSearchResult& SearchResult = Subsystem->GetActiveOnlineSessionSearch()->SearchResults[i - UMenuJoinSessionFrontEnd::UsedSBIndices];

        if (SearchResult.IsValid() == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: Invalid search result at index %d."), i - UMenuJoinSessionFrontEnd::UsedSBIndices)
            continue;
        }

        if (SearchResult.IsSessionInfoValid() == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: Invalid session info at index %d."), i - UMenuJoinSessionFrontEnd::UsedSBIndices)
            continue;
        }

        FOnlineSession OnlineSession = SearchResult.Session;
        FOnlineSessionSettings SessionSettings = OnlineSession.SessionSettings;
        
        FOnlineSessionEntryData EntryData = FOnlineSessionEntryData();
        
        EntryData.EntryIndex = i;

        EntryData.IDStr      = SearchResult.GetSessionIdStr();
        EntryData.PingInMs   = SearchResult.PingInMs;

        EntryData.OwningUserID = OnlineSession.OwningUserId->ToString();
        EntryData.OwningUserName = OnlineSession.OwningUserName;
        EntryData.NumOpenPrivateConnections = OnlineSession.NumOpenPrivateConnections;
        EntryData.NumOpenPublicConnections = OnlineSession.NumOpenPublicConnections;

        EntryData.MaxPublicConnections = SessionSettings.NumPublicConnections;
        EntryData.MaxPrivateConnections = SessionSettings.NumPrivateConnections;
        EntryData.bLANSession = SessionSettings.bIsLANMatch;
        EntryData.bDedicatedSession = SessionSettings.bIsDedicated;
        
        UJAFGCommonWidget* Entry = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WB_OnlineSessionEntryClass);
        Entry->PassDataToWidget(EntryData);
        this->SB_FoundSessions->AddChild(Entry);

        continue;
    }

    return;
}

void UMenuJoinSessionFrontEnd::ReloadFoundSessions()
{
#if WITH_EDITOR
#if WITH_EDITORONLY_DATA
    if (this->bMockOnlineSubsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("UMenuJoinSessionFrontEnd::ReloadFoundSessions: Mocking online subsystem."))
        
        this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Collapsed);
        this->O_LoadingSessionsPlaceholder->SetVisibility(ESlateVisibility::Collapsed);
        
        for (int i = UMenuJoinSessionFrontEnd::UsedSBIndices; i < this->MockSessionCount + UMenuJoinSessionFrontEnd::UsedSBIndices; ++i)
        {
            FOnlineSessionEntryData MockSessionEntryData; UMenuJoinSessionFrontEnd::MakeMockSessionEntryData(i, MockSessionEntryData);
            
            UJAFGCommonWidget* Entry = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WB_OnlineSessionEntryClass);
            Entry->PassDataToWidget(MockSessionEntryData);
            this->SB_FoundSessions->AddChild(Entry);
        
            continue;
        }
        
        return;
    }
#endif /* WITH_EDITORONLY_DATA */
#endif /* WITH_EDITOR */
    
    constexpr uint32 MaxSearchResults = 0x10;
    constexpr bool bLANQuery = true;

    const UGameInstance* GameInstance = this->GetGameInstance(); check ( GameInstance )
    ULocalSessionSupervisorSubsystem* LSSS = GameInstance->GetSubsystem<ULocalSessionSupervisorSubsystem>(); check( LSSS )

    const TScriptInterface<IOnlineSessionSearchCallback> SearchCallbackPtr = TScriptInterface<IOnlineSessionSearchCallback>(this);

    LSSS->FindSessionsAndSafeDiscardPrevious(MaxSearchResults, bLANQuery, SearchCallbackPtr);

    this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Collapsed);
    this->O_LoadingSessionsPlaceholder->SetVisibility(ESlateVisibility::Visible);
    
    return;
}
