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

    FJAFGInputAction ToggleEscapeMenuAction;
    ToggleEscapeMenuAction.Name = InputActions::ToggleEscapeMenu;
    ToggleEscapeMenuAction.DefaultKeyA = EKeys::Escape;
    ToggleEscapeMenuAction.DefaultKeyB = EKeys::Invalid;
    ToggleEscapeMenuAction.Contexts.Add(InputContexts::Escape);
    ToggleEscapeMenuAction.Contexts.Add(InputContexts::Foot);
    InputSubsystem->AddAction(ToggleEscapeMenuAction);

    return;
}
