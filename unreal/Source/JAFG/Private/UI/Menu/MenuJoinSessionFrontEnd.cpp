// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/MenuJoinSessionFrontEnd.h"

#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "UI/Menu/OnlineSessionEntry.h"

void UMenuJoinSessionFrontEnd::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete(const bool bSuccess, const ULocalSessionSupervisorSubsystem* Subsystem)
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
        UE_LOG(LogTemp, Fatal, TEXT("UMenuJoinSessionFrontEnd::OnOnlineSessionFoundComplete: World is nullptr."))
        return;
    }
    
    this->O_EmptySessionPlaceholder->SetVisibility(ESlateVisibility::Collapsed);

    for (int32 i = 0; i < Subsystem->GetActiveOnlineSessionSearch()->SearchResults.Num(); ++i)
    {
        FOnlineSessionEntryData EntryData = FOnlineSessionEntryData();
        EntryData.OnlineSessionEntryIndex = i;
        EntryData.OnlineSessionName = Subsystem->GetActiveOnlineSessionSearch()->SearchResults[i].Session.OwningUserName;

        UJAFGCommonWidget* Entry = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WB_OnlineSessionEntryClass);
        Entry->PassDataToWidget(EntryData);
        this->SB_FoundSessions->AddChild(Entry);

        continue;
    }

    this->OnDeferredConstruct();
    
    return;
}

void UMenuJoinSessionFrontEnd::ReloadFoundSessions()
{
    constexpr uint32 MaxSearchResults = 0x10;
    constexpr bool bLANQuery = true;

    const UGameInstance* GameInstance = this->GetGameInstance(); check ( GameInstance )
    ULocalSessionSupervisorSubsystem* LSSS = GameInstance->GetSubsystem<ULocalSessionSupervisorSubsystem>(); check( LSSS )

    TScriptInterface<IOnlineSessionSearchCallback> SearchCallbackPtr = TScriptInterface<IOnlineSessionSearchCallback>(this);

    LSSS->FindSessionsAndSafeDiscardPrevious(MaxSearchResults, bLANQuery, SearchCallbackPtr);

    /* Maybe hit some loading screen? */
    
    return;
}
