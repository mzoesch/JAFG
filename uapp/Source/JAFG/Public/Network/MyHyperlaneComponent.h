// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "HyperlaneComponent.h"

#include "MyHyperlaneComponent.generated.h"

JAFG_VOID

namespace TransmittableData
{
struct FChunkInitializationData;
}

class UChunkGenerationSubsystem;

UCLASS(NotBlueprintable)
class JAFG_API UMyHyperlaneComponent : public UHyperlaneComponent
{
    GENERATED_BODY()

public:

    explicit UMyHyperlaneComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay(void) override;

protected:

    virtual bool MyAskServerForVoxelData_ServerRPC_Validate(const FChunkKey& ChunkKey) override;
    virtual void MyAskServerForVoxelData_ServerRPC_Implementation(const FChunkKey& ChunkKey) override;

    virtual void OnChunkInitializationDataReceived(const TransmittableData::FChunkInitializationData& Data) override;

    UPROPERTY()
    TObjectPtr<UChunkGenerationSubsystem> ChunkGenerationSubsystem = nullptr;
};
