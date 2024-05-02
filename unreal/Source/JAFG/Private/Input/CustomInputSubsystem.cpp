// Copyright 2024 mzoesch. All rights reserved.

#include "Input/CustomInputSubsystem.h"

#include "Input/CustomInputNames.h"
#include "Input/JAFGInputSubsystem.h"

void UCustomInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UJAFGInputSubsystem>();
    Super::Initialize(Collection);

    LOG_ERROR(LogGameSettings, "Called.")

    this->AddAllKeyMappings();

    return;
}

void UCustomInputSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UCustomInputSubsystem::AddAllKeyMappings(void) const
{
    LOG_VERBOSE(LogGameSettings, "Called.")

     UJAFGInputSubsystem* InputSubsystem = this->GetLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();

#if WITH_EDITOR
    if (InputSubsystem == nullptr)
    {
        LOG_FATAL(LogGameSettings, "JAFG Input subsystem is invalid.")
        return;
    }
#endif /* WITH_EDITOR */

    FJAFGInputContext EscapeMenuContext;
    EscapeMenuContext.Name = InputContexts::Escape;
    InputSubsystem->AddContext(EscapeMenuContext);

    FJAFGInputContext FootContext;
    FootContext.Name = InputContexts::Foot;
    InputSubsystem->AddContext(FootContext);

    // Toggle Escape Menu
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::ToggleEscapeMenu;
        Action.DefaultKeyA = EKeys::Escape;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Escape);
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Jump
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::Jump;
        Action.DefaultKeyA = EKeys::SpaceBar;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    return;
}
