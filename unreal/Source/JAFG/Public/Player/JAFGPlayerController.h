// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "JAFGPlayerController.generated.h"

class ACommonChunk;

USTRUCT()
struct FInitialChunkData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<int> RawVoxels;
};

/** Super abstract base class for all player controllers. */
UCLASS(Abstract)
class JAFG_API AJAFGPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:

    explicit AJAFGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void ShowMouseCursor(const bool bShow, const bool bCenter = true);

public:

    /** Must be called on the server. */
    void SetInitialChunkDataForClientAsync(ACommonChunk* TargetChunk);

private:

    UFUNCTION(Client, Reliable)
    void SetInitialChunkDataForClient_ClientRPC(const FInitialChunkData& InitialChunkData);
};
