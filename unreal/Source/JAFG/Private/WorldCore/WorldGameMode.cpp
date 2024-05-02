// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldGameMode.h"

#include "JAFG/Public/Player/WorldPlayerController.h"
#include "JAFG/Public/WorldCore/WorldCharacter.h"

AWorldGameMode::AWorldGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PlayerControllerClass = AWorldPlayerController::StaticClass();
    this->DefaultPawnClass = AWorldCharacter::StaticClass();
}
