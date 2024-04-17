// Copyright 2024 mzoesch. All rights reserved.

#include "GenPrevAssets/GenPrevAssetsCharacter.h"

#include "GenPrevAssets/GenPrevAssetsComponent.h"

AGenPrevAssetsCharacter::AGenPrevAssetsCharacter(const FObjectInitializer& ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->GenPrevAssetsComponent = CreateDefaultSubobject<UGenPrevAssetsComponent>(TEXT("GenPrevAssetsComponent"));

    check( this->GenPrevAssetsComponent )

    return;
}

void AGenPrevAssetsCharacter::BeginPlay(void)
{
    Super::BeginPlay();
}

void AGenPrevAssetsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

