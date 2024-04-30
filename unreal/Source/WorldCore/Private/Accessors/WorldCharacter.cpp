// Copyright 2024 mzoesch. All rights reserved.


#include "Accessors/WorldCharacter.h"


// Sets default values
AWorldCharacter::AWorldCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWorldCharacter::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AWorldCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

