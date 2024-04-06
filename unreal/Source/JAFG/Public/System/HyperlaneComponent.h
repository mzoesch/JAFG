// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HyperlaneComponent.generated.h"

class FHyperlaneWorker;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAFG_API UHyperlaneComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UHyperlaneComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual ~UHyperlaneComponent() override;

protected:

    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void InitializeChunkWithAuthorityData(FIntVector ChunkKey, TArray<int32> Voxels);

private:

    FHyperlaneWorker* Worker = nullptr;
};
