// Copyright 2024 mzoesch. All rights reserved.

#include "Editor/EditorWorldCommandsSimulation.h"

#include "UI/WorldHUD.h"
#include "UI/OSD/Debug/DebugScreen.h"

void UEditorWorldCommandsSimulation::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UEditorWorldCommandsSimulation::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

#if WITH_EDITOR
    return  GEditor && GEditor->IsSimulateInEditorInProgress();
#else /* WITH_EDITOR */
    return false;
#endif /* !WITH_EDITOR */
}

void UEditorWorldCommandsSimulation::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    this->RegisterConsoleCommands();
    return;
}

void UEditorWorldCommandsSimulation::Deinitialize(void)
{
    Super::Deinitialize();
    this->UnregisterConsoleCommands();
    return;
}

void UEditorWorldCommandsSimulation::ToggleDebugScreen(void) const
{
#if WITH_EDITOR
    Cast<UDebugScreen>(Cast<AWorldHUD>(
        GEngine->GetGamePlayer(this->GetWorld(), 0)->GetPlayerController(this->GetWorld()
    )->GetHUD())->DebugScreen)->ToggleInSimulateMode();
#else /* WITH_EDITOR */
    LOG_FATAL(LogEditorCommands, "Disallowed call outside of editor.")
#endif /* !WITH_EDITOR */

    return;
}

void UEditorWorldCommandsSimulation::RegisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Registering console commands")

    this->ToggleDebugScreenCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("j.ToggleDebugScreen"),
        TEXT("Toggles the debug screen on and off."),
        FConsoleCommandDelegate::CreateLambda( [this] (void) { this->ToggleDebugScreen(); } ),
        ECVF_Cheat
    );

    return;
}

void UEditorWorldCommandsSimulation::UnregisterConsoleCommands(void)
{
    LOG_DISPLAY(LogEditorCommands, "Unregistering console commands")

    if (this->ToggleDebugScreenCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(this->ToggleDebugScreenCommand);
        this->ToggleDebugScreenCommand = nullptr;
    }

    return;
}
