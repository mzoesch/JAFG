// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BackgroundChunkUpdaterComponent.generated.h"


class AWorldGeneratorInfo;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAFG_API UBackgroundChunkUpdaterComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UBackgroundChunkUpdaterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay() override;

public:

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    TQueue<FIntVector> ChunkInitializationQueue;

private:

    UPROPERTY()
    TObjectPtr<AWorldGeneratorInfo> WorldGeneratorInfo = nullptr;

    UFUNCTION(Server, Reliable)
    void AskServerToGenerateChunkForClient_ServerRPC(const FIntVector& ChunkKey);
};
