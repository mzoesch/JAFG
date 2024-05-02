// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldCharacter.h"

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
}

void AWorldCharacter::BeginPlay(void)
{
    Super::BeginPlay();
}

void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

