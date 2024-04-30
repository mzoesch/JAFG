// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/GameMode.h"

#include "WorldGameMode.generated.h"

JAFG_VOID

class AWorldCharacter;

UCLASS(Abstract, Blueprintable)
class WORLDCORE_API AWorldGameMode : public AGameMode
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AWorldCharacter> DefaultWorldCharacterClass;

public:

    explicit AWorldGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual auto BeginPlay(void) -> void override;
    virtual auto PostLogin(APlayerController* NewPlayer) -> void override;
};
