// Copyright 2024 mzoesch. All rights reserved.

#include "SubMenus/NewSessionFrontEnd.h"

#include "LocalSessionSupervisorSubsystem.h"
#include "Components/ScrollBox.h"
#include "SubMenus/Entries/LocalSaveEntry.h"

void UNewSessionFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();

    // We do not want to retrieve and construct all data on startup, as this widget is being loaded immediately after
    // application boot up. The user may never active this widget during the whole runtime of the current app session.

    this->NewSessionName       = UNewSessionFrontEnd::DefaultSessionName;
    this->MaxPublicConnections = UNewSessionFrontEnd::DefaultMaxPublicConnections;
    this->bLAN                 = UNewSessionFrontEnd::bDefaultLAN;

    return;
}

void UNewSessionFrontEnd::HostListenServerAsync(void) const
{
    // All checks should have been done prior to calling this function in the blueprint implementation of this widget.

    if (this->NewSessionName.IsEmpty())
    {
        LOG_FATAL(LogCommonSlate, "New session name is empty.")
        return;
    }

    if (this->NewSessionName.Len() > UNewSessionFrontEnd::GetMaxSessionNameLength())
    {
        LOG_FATAL(LogCommonSlate, "New session name is too long.")
        return;
    }

    if (this->MaxPublicConnections < 1)
    {
        LOG_FATAL(LogCommonSlate, "Max public connections is less than 1.")
        return;
    }

    if (this->MaxPublicConnections > UNewSessionFrontEnd::GetMaxPublicConnections())
    {
        LOG_FATAL(LogCommonSlate, "Max public connections is greater than %d.", UNewSessionFrontEnd::GetMaxPublicConnections())
        return;
    }

    if (
        ULocalSessionSupervisorSubsystem* LSSS = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
        LSSS->HostListenServer(this->NewSessionName, this->MaxPublicConnections, this->bLAN) == false
    )
    {
        LOG_FATAL(LogCommonSlate, "Failed to host listen server.")
        return;
    }

    return;
}

void UNewSessionFrontEnd::ReloadLocalSaves(void)
{
    if (this->WB_SaveSlotClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Save slot class is invalid.")
        return;
    }

    this->NewSessionName       = UNewSessionFrontEnd::DefaultSessionName;
    this->MaxPublicConnections = UNewSessionFrontEnd::DefaultMaxPublicConnections;
    this->bLAN                 = UNewSessionFrontEnd::bDefaultLAN;

    this->SB_LocalSaves->ClearChildren();

    for (int32 i = 0; i < 32; ++i)
    {
        FLocalSaveEntryData EntryData;
        EntryData.SaveEntryIndex = i;
        EntryData.SaveEntryName  = FString::Printf(TEXT("Save Slot %d"), i);

        UJAFGWidget* Entry = CreateWidget<UJAFGWidget>(this->GetWorld(), this->WB_SaveSlotClass);
        Entry->PassDataToWidget(EntryData);
        this->SB_LocalSaves->AddChild(Entry);

        continue;
    }

    this->OnDeferredConstruct();

    return;
}
