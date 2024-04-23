// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Cuboid.h"
#include "GameFramework/Actor.h"
#include "Jar/Accumulated.h"

#include "Drop.generated.h"

JAFG_VOID

class ADrop;
class AWorldCharacter;

UCLASS()
class ADropImpl : public ACuboid
{
    GENERATED_BODY()

public:

    explicit ADropImpl(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaSeconds) override;

private:

    TArray<AWorldCharacter*> OverlappingCharacters;

    /**
     * The time when we ignore sphere components overlaps after AActor creation in seconds.
     */
    inline static constexpr float InvincibleTime { 0.5f };

    /**
     * Extra threshold to give other AActors time to enter the sphere component
     * after already overlapping AActors during invincible time in seconds.
     */
    inline static constexpr float EpsilonInvincibleThreshold { 1.0f };

    /**
     * The time in seconds since the world was brought up for play floored to the lower integer.
     * Is zero after the sum of ADropImpl#InvincibleTime and ADropImpl#EpsilonInvincibleThreshold
     * has surpassed the delta of the current world time and this value.
     */
    int CreationTime = 0;

    /**
     * Will be true after the ADropImpl#InvincibleTime has passed. To allow new characters to overlap and call
     * the appropriate delegate, but still hold back overlapping characters that were already overlapping
     * during the invincible time.
     */
    bool bPrioritizeNewCharacters = false;

    auto OnWorldCharacterOverlapBegin(AWorldCharacter* OverlappedCharacter) -> void;
    auto OnWorldCharacterOverlapEnd(AWorldCharacter* OverlappedCharacter) -> void;
    friend ADrop;
};

/**
 * A very short-lived AActor that is spawned into the world when an item is dropped
 * and self-destructs itself at the end of the first tick.
 * It helps to combine common functionality for all dropped items.
 */
UCLASS(NotBlueprintable)
class JAFG_API ADrop : public AActor
{
    GENERATED_BODY()

public:

    explicit ADrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    FORCEINLINE auto SetAccumulatedIndex(const int32 InAccumulatedIndex) -> void
    {
        this->AccumulatedIndex = InAccumulatedIndex;
    }

    FORCEINLINE auto SetForceAfterDrop(const FVector& InForceAfterDrop) -> void
    {
        this->ForceAfterDrop = InForceAfterDrop;
    }

private:

    int32   AccumulatedIndex = Accumulated::Null.AccumulatedIndex;
    FVector ForceAfterDrop   = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ADropImpl> DropImpl;

    inline static constexpr int32 DefaultVoxelDropCuboidX { 10 };
    inline static constexpr int32 DefaultVoxelDropCuboidY { 10 };
    inline static constexpr int32 DefaultVoxelDropCuboidZ { 10 };
    inline static constexpr int32 DefaultVoxelDropConvexX { 10 };
    inline static constexpr int32 DefaultVoxelDropConvexY { 10 };
    inline static constexpr int32 DefaultVoxelDropConvexZ { 10 };

    auto CreateDropOverlapBeginDelegate(void) const -> void;
    auto CreateDropOverlapEndDelegate(void) const -> void;
};
