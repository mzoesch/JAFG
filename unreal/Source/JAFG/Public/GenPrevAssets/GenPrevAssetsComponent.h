// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenPrevAssetsComponent.generated.h"

DECLARE_DELEGATE(FOnGenPrevAssetsCompleteEventSignature);

UCLASS(NotBlueprintable)
class AGenPrevAssetsActor : public AActor
{
    GENERATED_BODY()

public:

    explicit AGenPrevAssetsActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaTime) override;

    FOnGenPrevAssetsCompleteEventSignature OnGenPrevAssetsCompleteEvent;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneCaptureComponent2D* CaptureComponent;

    /**
     * The threshold where all channels will be discarded if all
     * of them (except the alpha channel) are below it.
     */
    inline static constexpr int NonAlphaThreshold = 0b0111;

    /**
     * Where we actually spawn the Cuboid in the UWorld. The Capture Component Acts as the
     * relative pivot point for the newly spawned AActor.
     */
    inline static const FTransform PrevTransform =
        FTransform(FQuat(0.898527, 0.321476, 0.276089, 0.114360), FVector(620, 0, 15), FVector::OneVector);

    /**
     * The current index of the voxel being processed by the scene capture component.
     * A value of zero means failure as Common Voxels cannot be processed.
     */
    int32 CurrentVoxelIndex = 0;

    /**
     * The total voxel num calculated by the voxel subsystem.
     * Copied for faster access.
     */
    int32 VoxelNum = 0;

    /**
     * Generates a Texture2D from the current voxel index.
     */
    auto GenerateForCurrentPrevAsset(void) -> void;

    /** Original: ERawImageFormat::GetBytesPerPixel(OutImage.Format); */
    static auto GetBytesPerPixel(const ERawImageFormat::Type Format) -> int64;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAFG_API UGenPrevAssetsComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UGenPrevAssetsComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay(void) override;
private:

    UPROPERTY()
    TObjectPtr<AGenPrevAssetsActor> GenPrevAssetsActor;

};
