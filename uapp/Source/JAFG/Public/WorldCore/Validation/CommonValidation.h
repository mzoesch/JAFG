// Copyright 2024 mzoesch. All rights reserved.

#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "CommonValidation.generated.h"

JAFG_VOID

class UChunkGenerationSubsystem;

namespace Validation
{

FORCEINLINE TArray<FChunkKey2> GetAllChunksInDistance(const FChunkKey2& Center, const int32 Distance)
{
    const int PredictedVerticalChunkCount = (Distance * 2 + 1) * (Distance * 2 + 1);
    TArray<FChunkKey2> Out; Out.Reserve(PredictedVerticalChunkCount);

    auto MoveCursorRight = [] (const FIntVector2& CursorLocation) { return FIntVector2( CursorLocation.X + 1, CursorLocation.Y     ); };
    auto MoveCursorDown  = [] (const FIntVector2& CursorLocation) { return FIntVector2( CursorLocation.X,     CursorLocation.Y - 1 ); };
    auto MoveCursorLeft  = [] (const FIntVector2& CursorLocation) { return FIntVector2( CursorLocation.X - 1, CursorLocation.Y     ); };
    auto MoveCursorUp    = [] (const FIntVector2& CursorLocation) { return FIntVector2( CursorLocation.X,     CursorLocation.Y + 1 ); };
    const TArray<FIntVector2(*) (const FIntVector2&)> Moves =
          TArray<FIntVector2(*) (const FIntVector2&)>( { MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp } );

    int Cursor = 1;
    int CurrentMoveIndex = 0;
    int TimesToMove = 1;
    FChunkKey2 TargetPoint = Center;

    Out.Emplace(Center.X, Center.Y);

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                if (Cursor++ >= PredictedVerticalChunkCount)
                {
                    goto MethodEnd;
                }

                Out.Emplace(TargetPoint.X, TargetPoint.Y);

                continue;
            }

            continue;
        }

        ++TimesToMove;
        continue;
    }

    MethodEnd:

        checkCode(
            for (int i = 0; i < Out.Num(); ++i)
            {
                for (int j = i + 1; j < Out.Num(); ++j)
                {
                    check( Out[i] != Out[j] )
                }
            }
        )

        return Out;
}

}

struct FChunkLoadingParams
{
    int32 RenderDistance;
};

/** Base class for all validation subsystems. */
UCLASS(Abstract, NotBlueprintable)
class JAFG_API UChunkValidationSubsystemCommon : public UJAFGTickableWorldSubsystemNoDev
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemCommon();

    // WorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkValidationSubsystem, STATGROUP_Tickables)
    }
    // ~FTickableGameObject implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation

protected:

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<UChunkGenerationSubsystem> ChunkGenerationSubsystem;

    void LoadUnloadChunks(const TArray<FVector>& WorldLocations, const FChunkLoadingParams& Params) const;

    void TrySpawnLocalCharacter(void) const;
    void TrySpawnRemoteCharacters(void) const;

    /** @return True, if meaningful. */
    bool GetPredictedLocalPlayerLocation(FVector& Out) const;
    TArray<FVector> GetAllPredictedPlayerLocations(void) const;

    template<class T>
    auto GetLocalPlayerController(void) const -> T*;
};
