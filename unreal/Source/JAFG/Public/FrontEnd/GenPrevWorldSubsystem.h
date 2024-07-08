// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/WorldSubsystem.h"

#include "GenPrevWorldSubsystem.generated.h"

JAFG_VOID

class UTextureSubsystem;
class UVoxelSubsystem;
class UGenPrevWorldSubsystem;

DECLARE_DELEGATE(OnFinishedGeneratingAssetsDelegateSignature)

/** Helper AActor needed to generate the assets. Spawned by the Gen Prev World Subsystem. */
UCLASS(NotBlueprintable)
class JAFG_API AGenPrevAssets : public AActor
{
    GENERATED_BODY()

public:

    explicit AGenPrevAssets(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaSeconds) override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

    UPROPERTY()
    TObjectPtr<UGenPrevWorldSubsystem> GenPrevWorldSubsystem;

    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem;

    UPROPERTY()
    TObjectPtr<UTextureSubsystem> TextureSubsystem;

private:

    /** The threshold where all channels will be discarded if all of them (except the alpha channel) are below it. */
    static constexpr int NonAlphaThreshold = 0b0111;

    voxel_t    CurrentIndex  = 0;

    /** Values based on testing */
    FTransform PrevTransform = FTransform(FRotator(25.0, 45.0, 25.0), FVector(620, 0, 15), FVector::OneVector);

    static auto GetBytesPerPixel(const ERawImageFormat::Type Format) -> int64;
};

/**
 * Exists only in the GenPrevWorld level and generates the at runtime generated assets.
 * Will load into the Front End level when completed.
 */
UCLASS(NotBlueprintable)
class JAFG_API UGenPrevWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UGenPrevWorldSubsystem();

    // UWorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    virtual auto Tick(const float DeltaTime) -> void override;
    // ~UWorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkValidationSubsystem, STATGROUP_Tickables)
    }
    // ~FTickableGameObject implementation

    /** Call this method to tell the subsystem to handle the processes after generation. */
    OnFinishedGeneratingAssetsDelegateSignature OnFinishedGeneratingAssetsDelegate;

private:

    void BindToOnFinishedGeneratingAssetsDelegate(void);
};
