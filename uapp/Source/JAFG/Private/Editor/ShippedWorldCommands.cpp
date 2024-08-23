// Copyright 2024 mzoesch. All rights reserved.

#include "Editor/ShippedWorldCommands.h"

//////////////////////////////////////////////////////////////////////////
// Console variables

static TAutoConsoleVariable<bool> CVarShowChunkPreview(
    CV_CHUNK_PREVIEW,
    false,
    TEXT("Shows the states of chunks inside the debug screen if active."),
    ECVF_Default
);

static TAutoConsoleVariable<bool> CVarShowChunkBorders(
    CV_CHUNK_BORDERS,
    false,
    TEXT("Shows the borders of chunks in the world."),
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

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UShippedWorldCommands::ShowChunkBorders(void) const
{
    return CVarShowChunkBorders.GetValueOnAnyThread();
}

FDelegateHandle UShippedWorldCommands::AttachToConsoleVariable(const FString& ConsoleVariableName, const FConsoleVariableDelegate& Delegate)
{
    if (ConsoleVariableName.StartsWith("j.") == false)
    {
        LOG_FATAL(LogEditorCommands, "JAFG Console variable name must start with 'j.'. Cannot attach to console variable '%s'.", *ConsoleVariableName);
        return FDelegateHandle();
    }

    IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
    if (ConsoleVariable == nullptr)
    {
        LOG_FATAL(LogEditorCommands, "Cannot find console variable '%s'. Cannot attach to console variable.", *ConsoleVariableName);
        return FDelegateHandle();
    }

    return ConsoleVariable->OnChangedDelegate().Add(Delegate);
}

bool UShippedWorldCommands::DetachFromConsoleVariable(const FString& ConsoleVariableName, const FDelegateHandle& Handle)
{
    if (ConsoleVariableName.StartsWith("j.") == false)
    {
        LOG_FATAL(LogEditorCommands, "JAFG Console variable name must start with 'j.'. Cannot attach to console variable '%s'.", *ConsoleVariableName);
        return false;
    }

    IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
    if (ConsoleVariable == nullptr)
    {
        LOG_FATAL(LogEditorCommands, "Cannot find console variable '%s'. Cannot attach to console variable.", *ConsoleVariableName);
        return false;
    }

    return ConsoleVariable->OnChangedDelegate().Remove(Handle);
}

void UShippedWorldCommands::RegisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Registering shipped world commands.")
}

void UShippedWorldCommands::UnregisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Unregistering shipped world commands.")
}

