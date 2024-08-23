// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTypeDefs.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkDebugSplineSubsystem.generated.h"

class USplineComponent;

UCLASS(NotBlueprintable)
class AChunkDebugWorldSpline : public AActor
{
    GENERATED_BODY()

public:

    explicit AChunkDebugWorldSpline(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USplineComponent> SplineComponent;
};

/** Automatically detects if the chunk borders are shown and updates / spawns the debug splines accordingly. */
UCLASS(NotBlueprintable)
class JAFG_API UChunkDebugSplineSubsystem : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkDebugSplineSubsystem();

    // UWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize(void) override;
    // ~UWorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkDebugSplineSubsystem, STATGROUP_Tickables)
    }
    // ~FTickableGameObject implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    virtual auto IsTickable(void) const -> bool override { return this->bEnabled; }
    // ~UJAFGTickableWorldSubsystem implementation

private:

    void OnToggle(const bool bInEnabled);
    void OnShow(void);
    void OnHide(void);

    void UpdateAllSplines(const bool bForce = false);

    bool GetMostRespectedLocalPlayerLocation(FVector& OutLocation) const;

    UPROPERTY()
    TArray<AChunkDebugWorldSpline*> Splines;
    FChunkKey2      LastLocalPlayerLocation = FIntVector2(0, 0);
    bool            bEnabled                = false;
    FDelegateHandle OnCVarChangedHandle;
};
