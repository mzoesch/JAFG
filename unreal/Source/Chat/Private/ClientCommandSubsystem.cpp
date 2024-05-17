// Copyright 2024 mzoesch. All rights reserved.

#include "ClientCommandSubsystem.h"

#include "CommonNetworkStatics.h"

void UClientCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UClientCommandSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        return false;
    }
#endif /* WITH_EDITOR */

    return UNetStatics::IsSafeDedicatedServer(Outer) == false;
}

void UClientCommandSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

bool UClientCommandSubsystem::IsRegisteredClientCommand(const FText& StdIn)
{
    return false;
}

bool UClientCommandSubsystem::IsRegisteredClientCommand(const FClientCommand& Command) const
{
    return false;
}

void UClientCommandSubsystem::ExecuteCommand(const FText& StdIn, int32& OutErrorCode, FString& OutResponse) const
{
}

void UClientCommandSubsystem::ExecuteCommand(const FClientCommand& Command, const TArray<FString>& Args, int32& OutErrorCode, FString& OutResponse) const
{
}
