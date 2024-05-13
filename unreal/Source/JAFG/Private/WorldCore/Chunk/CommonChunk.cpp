// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/CommonChunk.h"

#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/ChunkWorldSettings.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;

    this->ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    /* We can do this for far away chunks, maybe? */
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    this->SetPrivateStateDelegateHandle_Invalid();

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    this->SetChunkState(EChunkState::PreSpawned);

    return;
}

void ACommonChunk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->bUncontrolledKill)
    {
        return;
    }

    return;
}

void ACommonChunk::PreInitialize(void)
{
    this->RawVoxelData.SetNum(WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize, false);
    this->JChunkPosition = this->GetActorLocation() * WorldStatics::UToJScale;
    this->ChunkKey       = FChunkKey(this->JChunkPosition / (WorldStatics::ChunkSize - 1));

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( this->VoxelSubsystem )

    this->MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();
    check( this->MaterialSubsystem )

    this->ServerChunkWorldSettings = this->GetWorld()->GetSubsystem<UServerChunkWorldSettings>();
    if (this->ServerChunkWorldSettings == nullptr && UNetStatics::IsSafeServer(this))
    {
        LOG_FATAL(LogChunkGeneration, "Could not get Server Chunk World Settings.")
        return;
    }

    return;
}

void ACommonChunk::KillUncontrolled(void)
{
    this->bUncontrolledKill = true;
    this->Destroy();

    return;
}

#pragma region Chunk State

void ACommonChunk::UnsubscribePrivateStateDelegateHandle()
{
    if (this->PrivateCurrentStateHandle.IsValid() == false)
    {
        LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event because the Hanlde was invalid.")
        this->KillUncontrolled();
        return;
    }
    if (this->UnsubscribeFromChunkStateChange(this->PrivateCurrentStateHandle) == false)
    {
        LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event due to an unknown error.")
        this->KillUncontrolled();
        return;
    }
    this->PrivateCurrentStateHandle.Reset();

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_Invalid(void)
{
    this->PrivateCurrentStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            this->UnsubscribePrivateStateDelegateHandle();

            if (NewChunkState != EChunkState::PreSpawned)
            {
                LOG_FATAL(LogChunkGeneration, "Waited for Pre Spawned state but got %s.", *EChunkState::LexToString(NewChunkState))
                return;
            }

            this->SetPrivateStateDelegateHandle_PreSpawned();

            return;
        }));

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_PreSpawned()
{
    this->PrivateCurrentStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            this->UnsubscribePrivateStateDelegateHandle();

           if (NewChunkState != EChunkState::Spawned)
           {
               LOG_FATAL(LogChunkGeneration, "Waited for Pre Spawned state but got %s.", *EChunkState::LexToString(NewChunkState))
               return;
           }

            this->PreInitialize();

            this->SetPrivateStateDelegateHandle_Spawned();

            return;
        }));

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_Spawned()
{
    this->PrivateCurrentStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            this->UnsubscribePrivateStateDelegateHandle();

            if (NewChunkState != EChunkState::Shaped)
            {
                LOG_FATAL(LogChunkGeneration, "Waited for Shaped state but got %s.", *EChunkState::LexToString(NewChunkState))
                return;
            }

            this->Shape();

            this->SetPrivateStateDelegateHandle_Shaped();

            return;
        }
    ));

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_Shaped()
{
    this->PrivateCurrentStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            this->UnsubscribePrivateStateDelegateHandle();

            if (NewChunkState != EChunkState::SurfaceReplaced)
            {
                LOG_FATAL(LogChunkGeneration, "Waited for Surface Replaced state but got %s.", *EChunkState::LexToString(NewChunkState))
                return;
            }

            this->ReplaceSurface();

            this->SetPrivateStateDelegateHandle_SurfaceReplaced();

            return;
        }
    ));

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_SurfaceReplaced(void)
{
    this->PrivateCurrentStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            this->UnsubscribePrivateStateDelegateHandle();

            if (NewChunkState != EChunkState::Active)
            {
                LOG_FATAL(LogChunkGeneration, "Waited for Active state but got %s.", *EChunkState::LexToString(NewChunkState))
                return;
            }

            this->RegenerateProceduralMesh();

            this->SetPrivateStateDelegateHandle_Active();

            return;
        }
    ));

    return;
}

void ACommonChunk::SetPrivateStateDelegateHandle_Active()
{
    // Why does this cause a crash?
    this->ChunkStateChangeEvent.AddLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            LOG_WARNING(LogChunkGeneration, "Yea.")
        }
    );

    return;
}

#pragma endregion Chunk State

#pragma region Procedural Mesh

void ACommonChunk::ApplyProceduralMesh(void)
{
    if (MaterialSubsystem == nullptr)
    {
        LOG_FATAL(LogChunkMisc, "Could not get Material Subsystem.")
        return;
    }

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicGroups[i]);
        this->ProceduralMeshComponent->CreateMeshSection(
            i,
            this->MeshData[i].Vertices,
            this->MeshData[i].Triangles,
            this->MeshData[i].Normals,
            this->MeshData[i].UV0,
            this->MeshData[i].Colors,
            this->MeshData[i].Tangents,
            true
        );

        continue;
    }

    return;
}

#pragma endregion Procedural Mesh

#pragma region Chunk World Generation

void ACommonChunk::Shape(void)
{
    switch (this->ServerChunkWorldSettings->GetWorldGenerationType())
    {
    case EWorldGenerationType::Default:
    {
        this->GenerateDefaultWorld();
        break;
    }
    case EWorldGenerationType::Superflat:
    {
        this->GenerateSuperFlatWorld();
        break;
    }
    default:
    {
        checkNoEntry()
        break;
    }
    }

    return;
}

void ACommonChunk::GenerateSuperFlatWorld(void)
{
    /*
     * We should make this modular with some params that maybe even the user can provide.
     */

    constexpr int StoneVoxel { 2 };
    constexpr int DirtVoxel  { 3 };
    constexpr int GrassVoxel { 4 };

    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;

                if (WorldZ < 10)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), StoneVoxel);
                    continue;
                }

                if (WorldZ < 11)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), DirtVoxel);
                    continue;
                }

                if (WorldZ == 11)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), GrassVoxel);
                    continue;
                }

                this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), ECommonVoxels::Air);

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::GenerateDefaultWorld(void)
{
    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        const float WorldX = this->JChunkPosition.X + X;

        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            const float WorldY = this->JChunkPosition.Y + Y;

            const float ContinentalnessNoiseValue = this->ServerChunkWorldSettings->NoiseContinentalness.GetNoise(WorldX, WorldY);

            const float ContinentalnessTargetHeight = NoiseSpline::GetTargetHeight(this->ServerChunkWorldSettings->ContinentalnessSpline, ContinentalnessNoiseValue);

            for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;
                const float CurrentPercentageWorldZ = WorldZ / this->ServerChunkWorldSettings->GetFakeHighestPoint();

                const float Density = CurrentPercentageWorldZ < ContinentalnessTargetHeight ? 1.0f : -1.0f;

                this->ModifyRawVoxelData(
                    FVoxelKey(X, Y, Z),
                      this->ServerChunkWorldSettings->NoiseWorld.GetNoise(WorldX, WorldY, WorldZ)
                    >
                      Density
                        ? ECommonVoxels::Air : ECommonVoxels::GetBaseVoxel()
                );
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::ReplaceSurface(void)
{
    switch (this->ServerChunkWorldSettings->GetWorldGenerationType())
    {
    case EWorldGenerationType::Default:
    {
        this->GenerateSurface();
        break;
    }
    case EWorldGenerationType::Superflat:
    {
        /*
         * As the superflat world does not require neighbors chunks to shape themselves and then create the surface
         * of this chunk based on them, we can just skip this step.
         */
        break;
    }
    default:
    {
        checkNoEntry()
        break;
    }
    }

    return;
}

void ACommonChunk::GenerateSurface(void)
{
    // checkNoEntry()
}

#pragma endregion Chunk World Generation
