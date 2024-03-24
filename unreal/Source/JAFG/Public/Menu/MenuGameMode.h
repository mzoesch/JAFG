// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "MenuGameMode.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFG_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	explicit AMenuGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
