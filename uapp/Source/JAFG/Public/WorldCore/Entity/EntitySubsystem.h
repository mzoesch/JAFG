// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "EntitySubsystem.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UEntitySubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    explicit UEntitySubsystem();

    // WorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    void CreateDrop(const FAccumulated& InAccumulated, const FVector& InLocation, const FVector& InForce, const float ForceMultiplier);
    void CreateDrop(const FAccumulated& InAccumulated, const FVector& InLocation, const FVector& InForce);
    void CreateDrop(const FAccumulated& InAccumulated, const FTransform& InSourceTransform);

    inline static constexpr float MinRandomForceMultiplier { 250000.0f };
    inline static constexpr float MaxRandomForceMultiplier { 500000.0f };

    static auto GetRandomForceVector(const float MinRangeXY, const float MaxRangeXY, const float MinRangeZ, const float MaxRangeZ) -> FVector;
    static auto GetRandomForceVector(void) -> FVector;
    static auto GetRandomUpwardForceVector(void) -> FVector;
    static auto GetRandomForceVector2D(void) -> FVector2d;
};
