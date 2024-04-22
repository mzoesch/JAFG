// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Cuboid.h"

#include "Drop.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API ADrop : public ACuboid
{
    GENERATED_BODY()

public:

    explicit ADrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual auto Tick(const float DeltaTime) -> void override;
            auto AddForce(const FVector& Force) const -> void;

protected:

    virtual auto OnSphereComponentOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult&
    ) -> void override;

    virtual auto OnSphereComponentOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex
    ) -> void override;

private:

    /* All measurements in Unreal Coordinate System. */

    inline static constexpr int32 DefaultDropCuboidX { 10 };
    inline static constexpr int32 DefaultDropCuboidY { 10 };
    inline static constexpr int32 DefaultDropCuboidZ { 10 };
    inline static constexpr int32 DefaultDropConvexX { 10 };
    inline static constexpr int32 DefaultDropConvexY { 10 };
    inline static constexpr int32 DefaultDropConvexZ { 10 };
};
