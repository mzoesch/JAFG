// Copyright 2024 mzoesch. All rights reserved.

#include "System/LocalSessionSupervisorSubsystem.h"

#include "System/JAFGInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"

void ULocalSessionSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->TempName = FName("JAFG-Session");
    
    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::Initialize: Initializing Local Session Supervisor Subsystem."))
    
    const IOnlineSubsystem* OSSModule = IOnlineSubsystem::Get();
    
    if (OSSModule == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::Initialize: Online Subsystem Module is invalid."))
        return;
    }

    this->OnlineSessionInterface = OSSModule->GetSessionInterface();

    if (this->OnlineSessionInterface == nullptr || this->OnlineSessionInterface.IsValid() == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSessionSupervisor::Initialize: Online Session Interface is invalid."))
        return;
    }

    this->OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULocalSessionSupervisorSubsystem::OnCreateSessionComplete);
    
    return;
}

void ULocalSessionSupervisorSubsystem::Deinitialize()
{
    Super::Deinitialize();

    UE_LOG(LogTemp, Log, TEXT("ULocalSessionSupervisor::Deinitialize: Deinitializing Local Session Supervisor Subsystem."))

    if (this->OnlineSessionInterface.IsValid() == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::Deinitialize: Online Session Interface is invalid."))
        return;
    }
    
    this->ForceActiveSessionDestroy();
    
    return;
}

void ULocalSessionSupervisorSubsystem::HostListenServer(void)
{
    if (this->OnlineSessionInterface->CreateSession(1, this->TempName, FOnlineSessionSettings()))
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::HostListenServer: Successfully created session."))
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::HostListenServer: Failed to create session."))
    }
}

void ULocalSessionSupervisorSubsystem::ForceActiveSessionDestroy(void)
{
    if (this->OnlineSessionInterface->DestroySession(this->TempName))
    {
        UE_LOG(LogTemp, Warning, TEXT("ULocalSessionSupervisor::ForceActiveSessionDestroy: Successfully destroyed session."))
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::ForceActiveSessionDestroy: Failed to destroy session."))
    }
}

void ULocalSessionSupervisorSubsystem::OnCreateSessionComplete(const FName SessionName, const bool bSuccess)
{
    UE_LOG(LogTemp, Error, TEXT("ULocalSessionSupervisor::OnCreateSessionComplete: Session [%s] created with some name."), *SessionName.ToString())
}
