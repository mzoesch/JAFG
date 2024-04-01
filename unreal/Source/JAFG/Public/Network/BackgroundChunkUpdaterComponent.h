// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BackgroundChunkUpdaterComponent.generated.h"

class AJAFGPlayerController;
class ACommonChunk;

UCLASS(ClassGroup=(Custom))
class JAFG_API UBackgroundChunkUpdaterComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UBackgroundChunkUpdaterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


public:
    
    void FillDataFromAuthorityAsync(ACommonChunk* TargetChunk);

private:

    UFUNCTION(Server, Reliable, WithValidation)
    void FillDataFromAuthority_ServerRPC(APlayerController* TargetPlayerController, ACommonChunk* TargetChunk);
};
