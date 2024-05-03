// Copyright 2024 mzoesch. All rights reserved.

#include "Input/CustomInputSubsystem.h"

#include "Input/CustomInputNames.h"
#include "Input/JAFGInputSubsystem.h"

void UCustomInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UJAFGInputSubsystem>();
    Super::Initialize(Collection);

    LOG_VERBOSE(LogGameSettings, "Called.")

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

    /*----------------------------------------------------------------------------
        Contexts
    ----------------------------------------------------------------------------*/

    // Escape Menu Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputContext Context;
        Context.Name = InputContexts::Escape;
        InputSubsystem->AddContext(Context);
    }

    // Foot Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputContext Context;
        Context.Name = InputContexts::Foot;
        InputSubsystem->AddContext(Context);
    }

    /*----------------------------------------------------------------------------
        Actions
    ----------------------------------------------------------------------------*/

    // Move Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFG2DInputAction Action;
        Action.Name             = InputActions::Move;
        Action.NorthDefaultKeyA = EKeys::W;
        Action.NorthDefaultKeyB = EKeys::Invalid;
        Action.SouthDefaultKeyA = EKeys::S;
        Action.SouthDefaultKeyB = EKeys::Invalid;
        Action.WestDefaultKeyA  = EKeys::A;
        Action.WestDefaultKeyB  = EKeys::Invalid;
        Action.EastDefaultKeyA  = EKeys::D;
        Action.EastDefaultKeyB  = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Look Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFG2DMouseInputAction Action;
        Action.Name = InputActions::Look;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Jump Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::Jump;
        Action.DefaultKeyA = EKeys::SpaceBar;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Escape Menu Action
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

    return;
}
