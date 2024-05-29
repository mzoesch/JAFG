// Copyright 2024 mzoesch. All rights reserved.

#include "Input/CustomInputSubsystem.h"

#include "Input/CustomInputNames.h"
#include "SettingsData/JAFGInputSubsystem.h"

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
        Upper Contexts
    ----------------------------------------------------------------------------*/

    // Footlike Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGUpperInputContext Context;
        Context.Name = InputContexts::Footlike;
        Context.InputContextRedirections.Add(InputContexts::FootWalk);
        Context.InputContextRedirections.Add(InputContexts::FootFly);
        InputSubsystem->AddUpperContext(Context);
    }

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

    // Foot Walk Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputContext Context;
        Context.Name = InputContexts::FootWalk;
        InputSubsystem->AddContext(Context);
    }

    // Foot Fly Context
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGInputContext Context;
        Context.Name = InputContexts::FootFly;
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
        FJAFGTwoDimensionalInputAction Action;
        Action.Name           = InputActions::Move;
        Action.NorthKeys.KeyA = EKeys::W;
        Action.NorthKeys.KeyB = EKeys::Invalid;
        Action.SouthKeys.KeyA = EKeys::S;
        Action.SouthKeys.KeyB = EKeys::Invalid;
        Action.WestKeys.KeyA  = EKeys::A;
        Action.WestKeys.KeyB  = EKeys::Invalid;
        Action.EastKeys.KeyA  = EKeys::D;
        Action.EastKeys.KeyB  = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Look Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGTwoDimensionalMouseInputAction Action;
        Action.Name = InputActions::Look;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Jump Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::Jump;
        Action.Keys.KeyA = EKeys::SpaceBar;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootWalk);
        InputSubsystem->AddAction(Action);
    }

    // Fly Up Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::FlyUp;
        Action.Keys.KeyA = EKeys::E;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootFly);
        InputSubsystem->AddAction(Action);
    }

    // Crouch Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::Crouch;
        Action.Keys.KeyA = EKeys::Q;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootWalk);
        InputSubsystem->AddAction(Action);
    }

    // Fly Down Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::FlyDown;
        Action.Keys.KeyA = EKeys::Q;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootFly);
        InputSubsystem->AddAction(Action);
    }

    // Primary Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::Primary;
        Action.Keys.KeyA = EKeys::LeftMouseButton;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Secondary Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::Secondary;
        Action.Keys.KeyA = EKeys::RightMouseButton;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Escape Menu Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::ToggleEscapeMenu;
        Action.Keys.KeyA = EKeys::Escape;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Escape);
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Debug Screen Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::ToggleDebugScreen;
#if WITH_EDITOR
        Action.Keys.KeyA = EKeys::RightAlt;
#else /* WITH_EDITOR */
        Action.Keys.KeyA = EKeys::F3;
#endif /* !WITH_EDITOR */
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Chat Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGDualInputAction Action;
        Action.Name = InputActions::ToggleChat;
        Action.Keys.Add( { EKeys::T,      EKeys::Invalid } );
        Action.Keys.Add( { EKeys::Escape, EKeys::Invalid } );
        Action.Contexts.Add( { InputContexts::Footlike } );
        Action.Contexts.Add( { InputContexts::Chat } );
        InputSubsystem->AddAction(Action);
    }

    // Toggle Quick Session Preview Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::ToggleQuickSessionPreview;
        Action.Keys.KeyA = EKeys::Tab;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Previous Chat StdIn Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::PreviousChatStdIn;
        Action.Keys.KeyA = EKeys::Up;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Chat);
        InputSubsystem->AddAction(Action);
    }

    // Next Chat StdIn Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::NextChatStdIn;
        Action.Keys.KeyA = EKeys::Down;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Chat);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Cameras Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::ToggleCameras;
#if WITH_EDITOR
        Action.Keys.KeyA = EKeys::Y;
#else /* WITH_EDITOR */
        Action.Keys.KeyA = EKeys::LeftShift;
#endif /* !WITH_EDITOR */
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Zoom Cameras Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::ZoomCameras;
        Action.Keys.KeyA = EKeys::C;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Toggle Zoom First-Person Camera Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::TogglePerspective;
        Action.Keys.KeyA = EKeys::O;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::Footlike);
        InputSubsystem->AddAction(Action);
    }

    // Up Max Fly Speed Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::UpMaxFlySpeed;
        Action.Keys.KeyA = EKeys::MouseScrollUp;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootFly);
        InputSubsystem->AddAction(Action);
    }

    // Up Max Fly Speed Action
    //////////////////////////////////////////////////////////////////////////
    {
        FJAFGSingleInputAction Action;
        Action.Name      = InputActions::DownMaxFlySpeed;
        Action.Keys.KeyA = EKeys::MouseScrollDown;
        Action.Keys.KeyB = EKeys::Invalid;
        Action.Contexts.Add(InputContexts::FootFly);
        InputSubsystem->AddAction(Action);
    }

    return;
}
