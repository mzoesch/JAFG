// Copyright 2024 mzoesch. All rights reserved.

#include "Editor/ShippedWorldCommands.h"

//////////////////////////////////////////////////////////////////////////
// Console variables

static TAutoConsoleVariable<bool> CVarShowChunkPreview(
    TEXT("j.ShowChunkPreview"),
    0.0f,
    TEXT("Shows the states of chunks inside the debug screen if active."),
    ECVF_Default
);

// ~Console variables
//////////////////////////////////////////////////////////////////////////

void UShippedWorldCommands::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UShippedWorldCommands::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}

void UShippedWorldCommands::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    this->RegisterConsoleCommands();
    return;
}

void UShippedWorldCommands::Deinitialize(void)
{
    Super::Deinitialize();
    this->UnregisterConsoleCommands();
    return;
}

UShippedWorldCommands* UShippedWorldCommands::Get(const UObject& Context)
{
    return Context.GetWorld()->GetSubsystem<UShippedWorldCommands>();
}

UShippedWorldCommands* UShippedWorldCommands::Get(const UObject* Context)
{
    return UShippedWorldCommands::Get(*Context);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UShippedWorldCommands::ShowChunkPreview(void) const
{
    return CVarShowChunkPreview.GetValueOnAnyThread();
}

void UShippedWorldCommands::RegisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Registering shipped world commands.")
}

void UShippedWorldCommands::UnregisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Unregistering shipped world commands.")
}

