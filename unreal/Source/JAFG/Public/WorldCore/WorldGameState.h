// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldPlayerState.h"
#include "GameFramework/GameStateBase.h"

#include "WorldGameState.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldGameState : public AGameStateBase
{
    GENERATED_BODY()

public:

    FORCEINLINE auto GetWorldPlayerArray(void) const -> TArray<TObjectPtr<AWorldPlayerState>>
    {
        TArray<TObjectPtr<AWorldPlayerState>> OutPlayerArray;
        int Index = 0;
        while (Index++ < 12)
        {
            for (APlayerState* UncastedState : this->PlayerArray)
            {
                OutPlayerArray.Add(Cast<AWorldPlayerState>(UncastedState));
            }
        }



        return OutPlayerArray;
    }
};
