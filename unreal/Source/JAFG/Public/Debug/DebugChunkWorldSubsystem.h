// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/WorldSubsystem.h"

#include "DebugChunkWorldSubsystem.generated.h"

JAFG_VOID

class USplineComponent;

UCLASS(NotBlueprintable)
class ADebugChunkWorldSpline : public AActor
{
    GENERATED_BODY()

public:

    explicit ADebugChunkWorldSpline(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USplineComponent> SplineComponent;
};

UCLASS(NotBlueprintable)
class JAFG_API UDebugChunkWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    explicit UDebugChunkWorldSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // World Subsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto PostInitialize(void) -> void override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~World Subsystem implementation

    // FTickableGameObject implementation
                virtual auto Tick(const float DeltaTime) -> void override;
    FORCEINLINE virtual auto GetStatId() const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UDebugChunkWorldSubsystem, STATGROUP_Tickables);
    }
    // ~FTickableGameObject implementation

private:

    IConsoleCommand* ShowChunkBordersCommand = nullptr;
    bool bShowChunkBorders = false;

    void OnToggleChunkBorders(void);
    /** Do not call directly. */
    void OnShowChunkBorders(void);
    /** Do not call directly. */
    void OnHideChunkBorders(void);

    UPROPERTY()
    TArray<ADebugChunkWorldSpline*> DebugChunkWorldSplines;
    FIntVector2 LastLocalPlayerPosition = FIntVector2(0, 0);
    void UpdateAllDebugSplines(const bool bForceUpdate = false);
};
