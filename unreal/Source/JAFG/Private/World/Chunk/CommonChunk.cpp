// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/CommonChunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Network/HyperlaneTransmitterInfo.h"
#include "Network/NetworkStatics.h"
#include "System/MaterialSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Chunk/ChunkMeshData.h"
#include "World/Voxel/CommonVoxels.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;

    this->ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    // We can do this for far away chunks maybe?
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeServer(this))
    {
        check( this->GetWorld() )
        check( this->GetWorld()->GetWorldSettings() )
        this->ChunkWorldSettings = Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
        check( this->ChunkWorldSettings )
        this->ChunkWorldSubsystem = this->GetWorld()->GetSubsystem<UChunkWorldSubsystem>();
        check( this->ChunkWorldSubsystem )
        this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));
        check (this->WorldGeneratorInfo )
    }
    else
    {
        if (Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass())))
        {
            UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::BeginPlay: Found World Generator Info on a client. This is disallowed."))
            return;
        }

        this->ChunkWorldSettings = nullptr;
        this->ChunkWorldSubsystem = nullptr;
        this->WorldGeneratorInfo = nullptr;
    }

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check(this->VoxelSubsystem)
    if (this->VoxelSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::BeginPlay: Could not get Voxel Subsystem."))
        return;
    }

    this->PreInitialize();

    if (UNetworkStatics::IsSafeServer(this))
    {
        this->GenerateVoxels();

        /* We here should of course only do convex meshing in the future. */
        this->GenerateProceduralMesh();
        this->ApplyProceduralMesh();

        return;
    }

    /*
     * Voxels are generated on the server and replicated to the client.
     * We do not have to care about that here.
     */

    return;
}

void ACommonChunk::PreInitialize(void)
{
    this->RawVoxels.SetNum(ChunkWorldSettings::ChunkSize * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::ChunkSize, false);
    this->JChunkPosition = this->GetActorLocation() * ChunkWorldSettings::UToJScale;
    this->ChunkKey = FIntVector(this->JChunkPosition / (ChunkWorldSettings::ChunkSize - 1));
}

void ACommonChunk::ApplyProceduralMesh(void) const
{
    const UMaterialSubsystem* MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();

    if (MaterialSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::ApplyProceduralMesh: Could not get Material Subsystem."))
        return;
    }

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        if (i == ETextureGroup::Opaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::Opaque, MaterialSubsystem->MDynamicOpaque);
        }
        else if (i == ETextureGroup::FullBlendOpaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FullBlendOpaque,
                                                       MaterialSubsystem->MDynamicFullBlendOpaque);
        }
        else if (i == ETextureGroup::FloraBlendOpaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FloraBlendOpaque,
                                                       MaterialSubsystem->MDynamicFloraBlendOpaque);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ACommonChunk::ApplyProceduralMesh: Texture group %d not implemented."), i)
            this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicOpaque);
        }

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
}

void ACommonChunk::ClearMesh()
{
    /* TODO We might want to use arr.empty? */

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        this->MeshData[i].Clear();
    }

    for (int i = 0; i < this->VertexCounts.Num(); ++i)
    {
        this->VertexCounts[i] = 0;
    }

    return;
}

void ACommonChunk::GenerateVoxels(void)
{
    check( this->GetWorld() )
    check( this->GetWorld()->GetWorldSettings() )
    check( Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings()) )

    switch (Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings())->WorldGenerationType)
    {
    case EWorldGenerationType::Default:
    {
        this->GenerateDefaultWorld();
        return;
    }
    case EWorldGenerationType::SuperFlat:
    {
        this->GenerateSuperFlatWorld();
        return;
    }
    default:
    {
        break;
    }
    }

    LOG_ERROR(LogChunkGeneration, "World Generation of type %s not implemented.",
        *ChunkWorldSettings::LexToString(Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings())->WorldGenerationType))
    this->HoldAutoShrinking();

    return;
}

void ACommonChunk::GenerateSuperFlatWorld()
{
    /*
     * We of course should make this modular with some params that maybe even the user can provide.
     */

    constexpr int StoneVoxel{2};
    constexpr int DirtVoxel{3};
    constexpr int GrassVoxel{4};

    for (int X = 0; X < ChunkWorldSettings::ChunkSize; ++X)
    {
        for (int Y = 0; Y < ChunkWorldSettings::ChunkSize; ++Y)
        {
            for (int Z = 0; Z < ChunkWorldSettings::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;

                if (WorldZ < 10)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = StoneVoxel;
                    continue;
                }

                if (WorldZ < 11)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = DirtVoxel;
                    continue;
                }

                if (WorldZ == 11)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = GrassVoxel;
                    continue;
                }

                this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = ECommonVoxels::Air;

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
    this->ShapeTerrain();
    this->ReplaceSurface();
}

void ACommonChunk::ShapeTerrain(void)
{
    check( ChunkWorldSettings )

    for (int X = 0; X < ChunkWorldSettings::ChunkSize; ++X)
    {
        const float WorldX = this->JChunkPosition.X + X;

        for (int Y = 0; Y < ChunkWorldSettings::ChunkSize; ++Y)
        {
            const float WorldY = this->JChunkPosition.Y + Y;

            const float ContinentalnessNoiseValue = this->ChunkWorldSettings->NoiseContinentalness->GetNoise(WorldX, WorldY);

            const float ContinentalnessTargetHeight = NoiseSpline::GetTargetHeight(this->ChunkWorldSettings->ContinentalnessSpline, ContinentalnessNoiseValue);

            for (int Z = 0; Z < ChunkWorldSettings::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;
                const float CurrentPercentageWorldZ = WorldZ / this->ChunkWorldSettings->GetFakeHighestPoint();

                const float Density = CurrentPercentageWorldZ < ContinentalnessTargetHeight ? 1.0f : -1.0f;

                this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))]
                    = this->ChunkWorldSettings->NoiseWorld->GetNoise(WorldX, WorldY, WorldZ)
                        > Density ? ECommonVoxels::Air : ECommonVoxels::GetBaseVoxel();
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::ReplaceSurface(void)
{
    constexpr int MaxModified     { 3 };
    constexpr int SurfaceVoxel    { 4 };
    constexpr int SubSurfaceVoxel { 3 };

    for (int X = 0; X < ChunkWorldSettings::ChunkSize; ++X)
    {
        for (int Y = 0; Y < ChunkWorldSettings::ChunkSize; ++Y)
        {
            int Modified = 0;

            /* The voxel on the bottom side of the above chunk. */
            const int TopVoxel = this->GetVoxelInNeighbourChunk(FIntVector(X, Y, ChunkWorldSettings::ChunkSize));

            if (TopVoxel == ECommonVoxels::GetBaseVoxel())
            {
                Modified = MaxModified;
            }

            else if (TopVoxel == SurfaceVoxel)
            {
                Modified = 1;
            }

            /* This is kinda sketchy because now there may be parts with more subsurface blocks. */
            else if (TopVoxel == SubSurfaceVoxel)
            {
                Modified = 2;
            }

            for (int Z = ChunkWorldSettings::ChunkSize -1 ; Z >= 0; --Z)
            {
                if (this->GetRawVoxelData(FIntVector(X, Y, Z)) == ECommonVoxels::Air)
                {
                    Modified = 0;
                    continue;
                }

                if (Modified >= MaxModified)
                {
                    continue;
                }

                this->ModifyRawVoxelData(FIntVector(X, Y, Z), Modified++ == 0 ? SurfaceVoxel : SubSurfaceVoxel);

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::SendInitializationDataToClient(const AWorldPlayerController* Target) const
{
    check ( Target )

    LOG_VERY_VERBOSE(LogChunkValidation, "Sending packaged initialization data to client for chunk %s.", *this->ChunkKey.ToString())

    AHyperlaneTransmitterInfo* Transmitter = Cast<AHyperlaneTransmitterInfo>(UGameplayStatics::GetActorOfClass(this, AHyperlaneTransmitterInfo::StaticClass()));
    check( Transmitter )

    TransmittableData::FChunkInitializationData Data = TransmittableData::FChunkInitializationData(this->ChunkKey, this->RawVoxels);
    Transmitter->SendChunkInitializationData(Target, Data);

    return;
}

void ACommonChunk::InitializeWithAuthorityData(const TArray<int32>& InRawVoxels)
{
    LOG_VERY_VERBOSE(LogChunkValidation, "Initializing chunk %s with authority data.", *this->ChunkKey.ToString())

    this->RawVoxels = InRawVoxels;

    this->ClearMesh();
    this->GenerateProceduralMesh();
    this->ApplyProceduralMesh();

    return;
}

ACommonChunk* ACommonChunk::GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only the server can get target chunks.")
        return nullptr;
    }

    FIntVector TransformedChunkKey   = this->ChunkKey;
    OutTransformedLocalVoxelPosition = LocalVoxelPosition;

    if (LocalVoxelPosition.X >= ChunkWorldSettings::ChunkSize)
    {
        TransformedChunkKey.X++;
        OutTransformedLocalVoxelPosition.X -= ChunkWorldSettings::ChunkSize;
    }

    else if (LocalVoxelPosition.X < 0)
    {
        TransformedChunkKey.X--;
        OutTransformedLocalVoxelPosition.X += ChunkWorldSettings::ChunkSize;
    }

    if (LocalVoxelPosition.Y >= ChunkWorldSettings::ChunkSize)
    {
        TransformedChunkKey.Y++;
        OutTransformedLocalVoxelPosition.Y -= ChunkWorldSettings::ChunkSize;
    }

    else if (LocalVoxelPosition.Y < 0)
    {
        TransformedChunkKey.Y--;
        OutTransformedLocalVoxelPosition.Y += ChunkWorldSettings::ChunkSize;
    }

    if (LocalVoxelPosition.Z >= ChunkWorldSettings::ChunkSize)
    {
        TransformedChunkKey.Z++;
        OutTransformedLocalVoxelPosition.Z -= ChunkWorldSettings::ChunkSize;
    }

    else if (LocalVoxelPosition.Z < 0)
    {
        TransformedChunkKey.Z--;
        OutTransformedLocalVoxelPosition.Z += ChunkWorldSettings::ChunkSize;
    }

    if (OutTransformedLocalVoxelPosition == LocalVoxelPosition)
    {
        return this;
    }

    LOG_VERBOSE(LogChunkManipulation, "Transformed local voxel position from %s to %s. Key change from %s to %s.",
        *LocalVoxelPosition.ToString(), *OutTransformedLocalVoxelPosition.ToString(), *this->ChunkKey.ToString(), *TransformedChunkKey.ToString())

    check( this->WorldGeneratorInfo )
    ACommonChunk* TargetChunk = this->WorldGeneratorInfo->GetChunkByKey(TransformedChunkKey);

    if (TargetChunk == nullptr)
    {
        return nullptr;
    }

    const FIntVector Temp = OutTransformedLocalVoxelPosition;
    return TargetChunk->GetTargetChunk(Temp, OutTransformedLocalVoxelPosition);
}

void ACommonChunk::ModifySingleVoxel(const FIntVector& LocalVoxelPosition, const int NewVoxel)
{
    LOG_VERY_VERBOSE(LogChunkManipulation, "Requested to modify single voxel at %s to %d in %s.",
        *LocalVoxelPosition.ToString(), NewVoxel, *this->ChunkKey.ToString())

    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only the server can modify voxels.")
        return;
    }

    FIntVector TransformedLocalVoxelPosition = LocalVoxelPosition;
    ACommonChunk* TargetChunk = this->GetTargetChunk(LocalVoxelPosition, TransformedLocalVoxelPosition);

    if (TargetChunk == nullptr)
    {
        LOG_ERROR(LogChunkManipulation, "Could not get target chunk for local voxel position %s (pivot: %s). Modfied to %s.",
            *LocalVoxelPosition.ToString(), *this->GetName(), *TransformedLocalVoxelPosition.ToString())
        return;
    }

    if (TargetChunk->GetRawVoxelData(TransformedLocalVoxelPosition) == NewVoxel)
    {
        LOG_WARNING(LogChunkManipulation, "Voxel at %s already has the value %d. No need to modify.",
            *TransformedLocalVoxelPosition.ToString(), NewVoxel)
        return;
    }

    TargetChunk->ModifyRawVoxelData(TransformedLocalVoxelPosition, NewVoxel);

    if (UNetworkStatics::IsSafeStandalone(this))
    {
        TargetChunk->RegenerateProceduralMesh();
        return;
    }

    this->ChunkWorldSubsystem->BroadcastChunkModification(TargetChunk->ChunkKey, TransformedLocalVoxelPosition, NewVoxel);

    /*
     * In the future we might want to look at this one more time
     * and try to only update a small percentage of the mesh.
     */
    /*
     * Here we of course should only do convex meshing.
     */
    TargetChunk->RegenerateProceduralMesh();

    return;
}

void ACommonChunk::ModifySingleVoxelOnClient(const FIntVector& LocalVoxelPosition, const int NewVoxel)
{
    LOG_VERY_VERBOSE(LogChunkManipulation, "Requested to modify single voxel at %s to %d in %s.",
        *LocalVoxelPosition.ToString(), NewVoxel, *this->ChunkKey.ToString())

    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Not a client. Disallowed.")
        return;
    }

    if (this->GetRawVoxelData(LocalVoxelPosition) == NewVoxel)
    {
        LOG_WARNING(LogChunkManipulation, "Voxel at %s already has the value %d. No need to modify.",
            *LocalVoxelPosition.ToString(), NewVoxel)
        return;
    }

    this->ModifyRawVoxelData(LocalVoxelPosition, NewVoxel);

    this->RegenerateProceduralMesh();

    return;
}

int32 ACommonChunk::GetVoxelInNeighbourChunk(const FIntVector& LocalVoxelPosition) const
{
    if (UNetworkStatics::IsServer(this) == false)
    {
        LOG_FATAL(LogChunkMisc, "Only the server can get voxels in neighbour chunks.")
        return ECommonVoxels::Null;
    }

    check( this->WorldGeneratorInfo )

    /*
     * We might want to cache the neighbour chunks.
     */
    if (LocalVoxelPosition.Z >= ChunkWorldSettings::ChunkSize)
    {
        if (ACommonChunk* Neighbour = this->WorldGeneratorInfo->GetChunkByKey(this->ChunkKey + FIntVector(0, 0, 1)))
        {
            return Neighbour->GetLocalVoxelOnly(FIntVector(LocalVoxelPosition.X, LocalVoxelPosition.Y, LocalVoxelPosition.Z - ChunkWorldSettings::ChunkSize));
        }

        return ECommonVoxels::Null;
    }

    LOG_FATAL(LogChunkMisc, "Not implemented.")
    return ECommonVoxels::Null;
}

FIntVector ACommonChunk::WorldToChunkKey(const FVector& WorldLocation)
{
    constexpr double Factor { ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScaleDouble };

    FIntVector ChunkKey;

    if (WorldLocation.X < 0)
    {
        ChunkKey.X = static_cast<int>(WorldLocation.X / Factor) - 1;
    }
    else
    {
        ChunkKey.X = static_cast<int>(WorldLocation.X / Factor);
    }

    if (WorldLocation.Y < 0)
    {
        ChunkKey.Y = static_cast<int>(WorldLocation.Y / Factor) - 1;
    }
    else
    {
        ChunkKey.Y = static_cast<int>(WorldLocation.Y / Factor);
    }

    if (WorldLocation.Z < 0)
    {
        ChunkKey.Z = static_cast<int>(WorldLocation.Z / Factor) - 1;
    }
    else
    {
        ChunkKey.Z = static_cast<int>(WorldLocation.Z / Factor);
    }

    return ChunkKey;
}

FIntVector ACommonChunk::WorldToLocalVoxelLocation(const FVector& WorldLocation)
{
    constexpr double Factor { ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScaleDouble };

    FIntVector ChunkLocation;

    if (WorldLocation.X < 0)
    {
        ChunkLocation.X = static_cast<int>(WorldLocation.X / Factor) - 1;
    }
    else
    {
        ChunkLocation.X = static_cast<int>(WorldLocation.X / Factor);
    }

    if (WorldLocation.Y < 0)
    {
        ChunkLocation.Y = static_cast<int>(WorldLocation.Y / Factor) - 1;
    }
    else
    {
        ChunkLocation.Y = static_cast<int>(WorldLocation.Y / Factor);
    }

    if (WorldLocation.Z < 0)
    {
        ChunkLocation.Z = static_cast<int>(WorldLocation.Z / Factor) - 1;
    }
    else
    {
        ChunkLocation.Z = static_cast<int>(WorldLocation.Z / Factor);
    }

    FIntVector LocalVoxelLocation =
          FIntVector(WorldLocation) / ChunkWorldSettings::JToUScaleInteger
        - ChunkLocation * ChunkWorldSettings::ChunkSize;

    /* Negative Normalization */

    if (ChunkLocation.X < 0)
    {
        --LocalVoxelLocation.X;
    }
    if (ChunkLocation.Y < 0)
    {
        --LocalVoxelLocation.Y;
    }
    if (ChunkLocation.Z < 0)
    {
        --LocalVoxelLocation.Z;
    }

    return LocalVoxelLocation;
}
