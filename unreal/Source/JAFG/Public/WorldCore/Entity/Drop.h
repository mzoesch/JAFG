// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Cuboid.h"

#include "Drop.generated.h"

JAFG_VOID

class AWorldCharacter;
class USphereComponent;

UCLASS(NotBlueprintable)
class JAFG_API ADrop : public ACuboid
{
    GENERATED_BODY()

public:

    explicit ADrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    bool bBeginPlayed = false;
    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaTime) override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> SphereComponent = nullptr;

public:

    /** Use this method instead of the equivalent super method. */
    void GenerateMesh(const FAccumulated& InAccumulated);
    void SetAccumulatedForDeferredMeshGeneration(const FAccumulated& InAccumulated) { this->CurrentAccumulated = InAccumulated; }

    void AddForce(const FVector& InForce) const;

protected:

    // ACuboid implementation
    virtual auto GetCurrentAccumulatedIndex(void) const -> voxel_t override { return this->CurrentAccumulated.AccumulatedIndex; }
    // ~ACuboid implementation

    UFUNCTION()
    void OnSphereComponentOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        const bool bFromSweep,
        const FHitResult& SweepResult
    );
    UFUNCTION()
    void OnSphereComponentOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex
    );

    /** The time when we ignore sphere components overlaps after AActor creation in seconds. */
    static constexpr float InvincibleTime { 0.5f };
    /**
     * Extra threshold to give other AActors time to enter the sphere component
     * after already overlapping AActors during invincible time in seconds.
     */
    static constexpr float EpsilonInvincibleThreshold { 1.0f };

    /** In seconds. */
    int32 CreationTime = 0;

    int32 ConvexX = 8;
    int32 ConvexY = 8;
    int32 ConvexZ = 8;

    int32 CollisionSphereRadius = 100;

    UPROPERTY(ReplicatedUsing=OnRep_CurrentAccumulated)
    FAccumulated CurrentAccumulated = Accumulated::Null;
    UFUNCTION()
    void OnRep_CurrentAccumulated( /* void */ );

    TArray<AWorldCharacter*> OverlappingCharacters;

private:

    inline static constexpr float ForceKillZ { -512.0f };

    // ACuboid implementation
    virtual void GenerateMesh(const voxel_t InAccumulated) override final;
    // ~ACuboid implementation
};
