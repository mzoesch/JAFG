// Copyright 2024 mzoesch. All rights reserved.

#include "Misc/WorldSimulationSpectatorPawn.h"

#include "EnhancedPlayerInput.h"
#include "UI/World/WorldHUD.h"
#include "World/Chunk/LocalChunkValidator.h"
#include "HAL/IConsoleManager.h"

#define PLAYER_CONTROLLER                                                                       \
    this->GetWorld()->GetFirstPlayerController()
#define HEAD_UP_DISPLAY                                                                         \
    Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD())
#define CHECKED_HEAD_UP_DISPLAY                                                                 \
    check( this->GetWorld() )                                                                   \
    check( this->GetWorld()->GetFirstPlayerController() )                                       \
    check( this->GetWorld()->GetFirstPlayerController()->GetHUD() )                             \
    check( Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD()) )            \
    HEAD_UP_DISPLAY
#define ENHANCED_INPUT_SUBSYSTEM                                                                \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(                             \
        Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController())->GetLocalPlayer() \
    )

AWorldSimulationSpectatorPawn::AWorldSimulationSpectatorPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->LocalChunkValidator = CreateDefaultSubobject<ULocalChunkValidator>(TEXT("LocalChunkValidator"));

    return;
}

void AWorldSimulationSpectatorPawn::BeginPlay(void)
{
    Super::BeginPlay();

    this->CommandToggleDebugScreen = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("j.dbs"),
        TEXT("Toggles the debug screen."),
        FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([&](const TArray<FString>& Args,UWorld* World)
        {
            this->OnToggleDebugScreen(FInputActionValue());
        }),
        ECVF_Cheat
    );

    return;
}

void AWorldSimulationSpectatorPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->CommandToggleDebugScreen)
    {
        IConsoleManager::Get().UnregisterConsoleObject(this->CommandToggleDebugScreen);
        this->CommandToggleDebugScreen = nullptr;
    }

    return;
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldSimulationSpectatorPawn::OnToggleDebugScreen(const FInputActionValue& Value)
{
    CHECKED_HEAD_UP_DISPLAY->ToggleDebugScreen();
}

#undef PLAYER_CONTROLLER
#undef HEAD_UP_DISPLAY
#undef CHECKED_HEAD_UP_DISPLAY
#undef ENHANCED_INPUT_SUBSYSTEM
