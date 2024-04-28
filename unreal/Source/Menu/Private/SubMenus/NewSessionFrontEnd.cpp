// Copyright 2024 mzoesch. All rights reserved.

#include "SubMenus/NewSessionFrontEnd.h"

#include "LocalSessionSupervisorSubsystem.h"

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

void UNewSessionFrontEnd::HostListenServerAsync(void)
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
