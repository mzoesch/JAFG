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

    // Chat Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputContext Context;
        Context.Name = InputContexts::Chat;
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
        Action.DefaultKeyB = EKeys::E;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Crouch Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::Crouch;
        Action.DefaultKeyA = EKeys::Q;
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

    // Toggle Debug Screen Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::ToggleDebugScreen;
#if WITH_EDITOR
        Action.DefaultKeyA = EKeys::RightAlt;
#else /* WITH_EDITOR */
        Action.DefaultKeyA = EKeys::F3;
#endif /* !WITH_EDITOR */
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Chat Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputActionDual Action;
        Action.Name        = InputActions::ToggleChat;
        Action.Keys.Add( { EKeys::T,      EKeys::Invalid } );
        Action.Keys.Add( { EKeys::Escape, EKeys::Invalid } );
        Action.Contexts.Add( { InputContexts::Foot } );
        Action.Contexts.Add( { InputContexts::Chat } );
        InputSubsystem->AddAction(Action);
    }

    // Previous Chat StdIn Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::PreviousChatStdIn;
        Action.DefaultKeyA = EKeys::Up;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Chat);
        InputSubsystem->AddAction(Action);
    }

    // Next Chat StdIn Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::NextChatStdIn;
        Action.DefaultKeyA = EKeys::Down;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Chat);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Cameras Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::ToggleCameras;
#if WITH_EDITOR
        Action.DefaultKeyA = EKeys::Y;
#else /* WITH_EDITOR */
        Action.DefaultKeyA = EKeys::LeftShift;
#endif /* !WITH_EDITOR */
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Zoom Cameras Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::ZoomCameras;
        Action.DefaultKeyA = EKeys::C;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Zoom First-Person Camera Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::TogglePerspective;
        Action.DefaultKeyA = EKeys::O;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Up Max Fly Speed Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::UpMaxFlySpeed;
        Action.DefaultKeyA = EKeys::MouseScrollUp;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    // Up Max Fly Speed Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputAction Action;
        Action.Name        = InputActions::DownMaxFlySpeed;
        Action.DefaultKeyA = EKeys::MouseScrollDown;
        Action.DefaultKeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Foot);
        InputSubsystem->AddAction(Action);
    }

    return;
}
