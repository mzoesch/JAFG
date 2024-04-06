// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"

#include "LocalPlayerChunkGenerator.generated.h"

UCLASS()
class JAFG_API ULocalPlayerChunkGenerator : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
};
