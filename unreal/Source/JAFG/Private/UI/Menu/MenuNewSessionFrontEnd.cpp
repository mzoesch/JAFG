// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/MenuNewSessionFrontEnd.h"

#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menu/LocalSaveEntry.h"
#include "System/LocalSessionSupervisorSubsystem.h"

void UMenuNewSessionFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();

    // We do not want to retrieve and construct all data on startup, as this widget is being loaded immediately after
    // application boot up. The user may never active this widget during the whole runtime of the current app session.

    this->NewSessionName       = UMenuNewSessionFrontEnd::DefaultSessionName;
    this->MaxPublicConnections = UMenuNewSessionFrontEnd::DefaultMaxPublicConnections;
    this->bLAN                 = UMenuNewSessionFrontEnd::bDefaultLAN;
    
    return;
}

void UMenuNewSessionFrontEnd::HostListenServerAsync() const
{
    /* TODO We of course have to provide some visual feedback to the user. */
    
    if (this->NewSessionName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("UMenuNewSessionFrontEnd::HostListenServerAsync: New session name is empty."));
        return;
    }

    if (this->NewSessionName.Len() > UMenuNewSessionFrontEnd::GetMaxSessionNameLength())
    {
        UE_LOG(LogTemp, Warning, TEXT("UMenuNewSessionFrontEnd::HostListenServerAsync: New session name is too long."));
        return;
    }

    if (this->MaxPublicConnections < 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("UMenuNewSessionFrontEnd::HostListenServerAsync: Max public connections is less than 1."));
        return;
    }

    if (this->MaxPublicConnections > ULocalSessionSupervisorSubsystem::MaxPublicConnections)
    {
        UE_LOG(LogTemp, Warning, TEXT("UMenuNewSessionFrontEnd::HostListenServerAsync: Max public connections is greater than %d."), ULocalSessionSupervisorSubsystem::MaxPublicConnections);
        return;
    }
    
    const UGameInstance* GameInstance = this->GetGameInstance(); check ( GameInstance )
    ULocalSessionSupervisorSubsystem* LSSS = GameInstance->GetSubsystem<ULocalSessionSupervisorSubsystem>(); check( LSSS )

    LSSS->HostListenServer(this->NewSessionName, this->MaxPublicConnections, this->bLAN);
    
    /* TODO Maybe hit some loading screen? */
    
    return;
}

void UMenuNewSessionFrontEnd::ReloadLocalSaves(void)
{
    check( this->WB_SaveSlotClass )
    
    this->NewSessionName       = UMenuNewSessionFrontEnd::DefaultSessionName;
    this->MaxPublicConnections = UMenuNewSessionFrontEnd::DefaultMaxPublicConnections;
    this->bLAN                 = UMenuNewSessionFrontEnd::bDefaultLAN;
    
    this->SB_LocalSaves->ClearChildren();
    
    for (int32 i = 0; i < 32; ++i)
    {
        FLocalSaveEntryData EntryData;
        EntryData.SaveEntryIndex = i;
        EntryData.SaveEntryName  = FString::Printf(TEXT("Save Slot %d"), i);
        
        UJAFGCommonWidget* Entry = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WB_SaveSlotClass);
        Entry->PassDataToWidget(EntryData);
        this->SB_LocalSaves->AddChild(Entry);

        continue;
    }

    this->OnDeferredConstruct();

    return;
}
