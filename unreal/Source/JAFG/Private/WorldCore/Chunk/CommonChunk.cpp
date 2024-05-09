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

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    this->ChunkState    = EChunkState::PreSpawned;
    this->SpawnedHandle = this->CreateOnSpawnedDelegateHandle(); check( this->SpawnedHandle.IsValid() )

    return;
}

void ACommonChunk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (bUncontrolledKill)
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

FDelegateHandle ACommonChunk::CreateOnSpawnedDelegateHandle(void)
{
    return this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            if (NewChunkState != EChunkState::Spawned)
            {
                LOG_ERROR(LogChunkGeneration, "Unknown chunk waiting for its minimal state to be set but got %s.", *EChunkState::LexToString(NewChunkState))
                this->KillUncontrolled();
                return;
            }

            this->PreInitialize();

            if (this->SpawnedHandle.IsValid() == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event because the Spawned Hanlde was invalid.")
                this->KillUncontrolled();
            }
            else if (this->UnsubscribeFromChunkStateChange(this->SpawnedHandle) == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event due to an unknown error.")
                this->KillUncontrolled();
            }

            /*
             * The chunk has now the spawned-state. Therefore, we now need to crate the new subscriber to listen for
             * the next step in the chunk's lifecycle.
             */
            this->ShapedHandle = this->CreateOnShapedDelegateHandle(); check( this->ShapedHandle.IsValid() )

            return;
        }
    ));
}

FDelegateHandle ACommonChunk::CreateOnShapedDelegateHandle(void)
{
    return this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            if (NewChunkState != EChunkState::Shaped)
            {
                LOG_ERROR(LogChunkGeneration, "Unknown chunk waiting for its shaped state to be set but got %s.", *EChunkState::LexToString(NewChunkState))
                this->KillUncontrolled();
                return;
            }

            this->Shape();

            if (this->ShapedHandle.IsValid() == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event because the Shaped Hanlde was invalid.")
                this->KillUncontrolled();
            }
            else if (this->UnsubscribeFromChunkStateChange(this->ShapedHandle) == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event due to an unknown error.")
                this->KillUncontrolled();
            }

            /*
             * The chunk has now the shaped-state. Therefore, we now need to crate the new subscriber to listen for
             * the next step in the chunk's lifecycle.
             */
            this->SurfaceReplacedHandle = this->CreateOnSurfaceReplacedDelegateHandle(); check( this->SurfaceReplacedHandle.IsValid() )

            return;
        }
    ));
}

FDelegateHandle ACommonChunk::CreateOnSurfaceReplacedDelegateHandle(void)
{
    return this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            if (NewChunkState != EChunkState::SurfaceReplaced)
            {
                LOG_ERROR(LogChunkGeneration, "Unknown chunk waiting for its surface replaced state to be set but got %s.", *EChunkState::LexToString(NewChunkState))
                this->KillUncontrolled();
                return;
            }

            this->ReplaceSurface();

            if (this->SurfaceReplacedHandle.IsValid() == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event because the Surface Replaced Hanlde was invalid.")
                this->KillUncontrolled();
            }
            else if (this->UnsubscribeFromChunkStateChange(this->SurfaceReplacedHandle) == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event due to an unknown error.")
                this->KillUncontrolled();
            }

            /*
             * The chunk has now the surface-replaced-state. Therefore, we now need to crate the new subscriber to listen
             * for the next step in the chunk's lifecycle.
             */
            this->ActiveHandle = this->CreateOnActiveDelegateHandle(); check( this->ActiveHandle.IsValid() )

            return;
        }
    ));
}

FDelegateHandle ACommonChunk::CreateOnActiveDelegateHandle(void)
{
    return this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState) -> void
        {
            if (NewChunkState != EChunkState::Active)
            {
                LOG_ERROR(LogChunkGeneration, "Unknown chunk waiting for its active state to be set but got %s.", *EChunkState::LexToString(NewChunkState))
                return;
            }

            this->RegenerateProceduralMesh();

            if (this->ActiveHandle.IsValid() == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event because the Active Hanlde was invalid.")
            }
            else if (this->UnsubscribeFromChunkStateChange(this->ActiveHandle) == false)
            {
                LOG_ERROR(LogChunkGeneration, "Failed to unsubscribe from Chunk State Change Event due to an unknown error.")
            }

            return;
        }
    ));
}

void ACommonChunk::SetChunkState(const EChunkState::Type NewChunkState)
{
    this->ChunkState = NewChunkState;

    this->ChunkStateChangeEvent.Broadcast(NewChunkState);

    return;
}

#pragma endregion Chunk State

#pragma region Procedural Mesh

void ACommonChunk::ApplyProceduralMesh(void)
{
    const UMaterialSubsystem* MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();

    if (MaterialSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::ApplyProceduralMesh: Could not get Material Subsystem."))
        return;
    }

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicGroups[i]);

        // if (i == ETextureGroup::Opaque)
        // {
        //     this->ProceduralMeshComponent->SetMaterial(ETextureGroup::Opaque, MaterialSubsystem->MDynamicOpaque);
        // }
        // else if (i == ETextureGroup::FullBlendOpaque)
        // {
        //     this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FullBlendOpaque,
        //                                                MaterialSubsystem->MDynamicFullBlendOpaque);
        // }
        // else if (i == ETextureGroup::FloraBlendOpaque)
        // {
        //     this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FloraBlendOpaque,
        //                                                MaterialSubsystem->MDynamicFloraBlendOpaque);
        // }
        // else
        // {
        //     UE_LOG(LogTemp, Error, TEXT("ACommonChunk::ApplyProceduralMesh: Texture group %d not implemented."), i)
        //     this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicOpaque);
        // }

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
    checkNoEntry()
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
    checkNoEntry()
}

#pragma endregion Chunk World Generation
