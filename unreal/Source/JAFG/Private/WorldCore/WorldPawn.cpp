// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/WorldPawn.h"

#include "System/JAFGGameInstance.h"
#include "RegisteredWorldNames.h"
#include "WorldCore/WorldGameMode.h"
#include "Player/WorldPlayerController.h"

AWorldPawn::AWorldPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
}

void AWorldPawn::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->GetWorld()->GetName() == RegisteredWorlds::Dev)
    {
        this->SetActorTickEnabled(true);
        return;
    }

    if (this->IsLocallyControlled() == false)
    {
        this->SetActorTickEnabled(false);
    }

    return;
}

void AWorldPawn::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
    this->GetGameTimeSinceCreation() > Timeout ? this->TimeoutDestroy() : void();

    if (this->GetWorld()->GetAuthGameMode() && this->GetController<AWorldPlayerController>())
    {
        this->GetWorld()->GetAuthGameMode<AWorldGameMode>()->SpawnCharacterForPlayer(this->GetController<AWorldPlayerController>());
    }

    return;
}

void AWorldPawn::TimeoutDestroy(void)
{
    LOG_ERROR(LogSystem, "Timed out.")
    this->GetGameInstance<UJAFGGameInstance>()->RequestControlledShutdown();
    this->Destroy();

    return;
}
