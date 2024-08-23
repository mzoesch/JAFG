// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/CommonChunk.h"
#include "HyperlaneComponent.h"
#include "Network/ChunkMulticasterInfo.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Validation/ChunkGenerationSubsystem.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;

    this->ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    /* TODO We currently use this, and it does not seem to cause any issues. But we have to investigate this further. */
    this->ProceduralMeshComponent->bUseAsyncCooking = true;
    this->ProceduralMeshComponent->SetCastShadow(true);
    this->ProceduralMeshComponent->bUseComplexAsSimpleCollision = false;

    this->SubscribeWithPrivateStateDelegate();

    return;
}

ACommonChunk::~ACommonChunk(void)
{
    if (this->RawVoxelData)
    {
        delete[] this->RawVoxelData;
        this->RawVoxelData = nullptr;
    }

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    /* Intentional. Never call side effects on first state change. */
    this->ChunkState = EChunkState::Freed;

    return;
}

void ACommonChunk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void ACommonChunk::OnAllocate(const FChunkKey& InChunkKey)
{
    check( this->GetChunkState() == EChunkState::Freed )

#if LOG_PERFORMANCE_CRITICAL_SECTIONS
    LOG_VERY_VERBOSE(LogChunkGeneration, "Spawning chunk at %s.", *ChunkKey.ToString())
#endif /* LOG_PERFORMANCE_CRITICAL_SECTIONS */

    const FTransform TargetedChunkTransform = FTransform(
        FRotator::ZeroRotator,
        FVector(
            InChunkKey.X * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            InChunkKey.Y * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            InChunkKey.Z * WorldStatics::ChunkSize * WorldStatics::JToUScale
        ),
        FVector::OneVector
    );

    this->SetActorTransform(TargetedChunkTransform, false, nullptr, ETeleportType::ResetPhysics);

    this->SetChunkState(EChunkState::PreSpawned);

    return;
}

void ACommonChunk::OnFree(void)
{
    this->ClearProceduralMesh();
    this->ApplyProceduralMesh();

    if (this->RawVoxelData)
    {
        delete[] this->RawVoxelData;
        this->RawVoxelData = nullptr;
    }

    /* Make sure we do not have any dangling pointers. */
    if (this->NNorth) { this->NNorth->NSouth = nullptr; } this->NNorth = nullptr;
    if (this->NEast)  { this->NEast->NWest   = nullptr; } this->NEast  = nullptr;
    if (this->NSouth) { this->NSouth->NNorth = nullptr; } this->NSouth = nullptr;
    if (this->NWest)  { this->NWest->NEast   = nullptr; } this->NWest  = nullptr;
    if (this->NUp)    { this->NUp->NDown     = nullptr; } this->NUp    = nullptr;
    if (this->NDown)  { this->NDown->NUp     = nullptr; } this->NDown  = nullptr;

    this->ChunkKey       = FChunkKey::ZeroValue;
    this->JChunkPosition = FVector::ZeroVector;

    this->SetChunkState(EChunkState::Freed);

    return;
}

void ACommonChunk::OnObliterate(void)
{
    check( this->GetChunkState() == EChunkState::Freed )

    this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>()->OnChunkWasKilledExternally(this->GetChunkKeyOnTheFly(true));
    this->Destroy();

    return;
}

void ACommonChunk::InitializeCommonStuff(void)
{
#if WITH_EDITOR
    /* PIE may not always clean up correctly. */
    if (this->RawVoxelData)
    {
        delete[] this->RawVoxelData;
        this->RawVoxelData = nullptr;
    }
#endif /* WITH_EDITOR */

    this->RawVoxelData = new voxel_t[WorldStatics::ChunkSizeCubed];
    FMemory::Memset(this->RawVoxelData, ECommonVoxels::Null, WorldStatics::ChunkSizeCubed * sizeof(voxel_t));

    this->JChunkPosition = this->GetActorLocation() * WorldStatics::UToJScale;
    this->ChunkKey       = WorldStatics::WorldToChunkKey(this->GetActorLocation());

    this->NNorth = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X + 1, this->ChunkKey.Y    , this->ChunkKey.Z    ));
    this->NEast  = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X    , this->ChunkKey.Y + 1, this->ChunkKey.Z    ));
    this->NSouth = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X - 1, this->ChunkKey.Y    , this->ChunkKey.Z    ));
    this->NWest  = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X    , this->ChunkKey.Y - 1, this->ChunkKey.Z    ));
    this->NUp    = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X    , this->ChunkKey.Y    , this->ChunkKey.Z + 1));
    this->NDown  = this->GetChunkGenerationSubsystem()->FindChunkByKey(FChunkKey(this->ChunkKey.X    , this->ChunkKey.Y    , this->ChunkKey.Z - 1));
    check( this->NNorth ) check( this->NEast ) check( this->NSouth ) check( this->NWest  )
    /* We do not check up and down, as this might be to most top or the most bottom chunk that will never have adjacent chunks in this direction. */

    return;
}

#pragma region Chunk State

void ACommonChunk::SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive)
{
    this->ChunkPersistency = NewPersistency;

    if (this->ChunkPersistency == EChunkPersistency::Persistent)
    {
        return;
    }

    if (
        const double EndOfLife = this->GetWorld()->GetRealTimeSeconds() + TimeToLive;
        EndOfLife > this->RealTimeInSecondsWhenTemporaryChunkShouldBeKilled
    )
    {
        this->RealTimeInSecondsWhenTemporaryChunkShouldBeKilled = EndOfLife;
    }

    return;
}

void ACommonChunk::SubscribeWithPrivateStateDelegate(void)
{
    if (this->PrivateStateHandle.IsValid())
    {
        return;
    }

    this->PrivateStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState)
        {
            if (this->GetChunkState() == EChunkState::Spawned)
            {
                this->OnSpawned();
            }
            else if (this->GetChunkState() == EChunkState::Shaped)
            {
                this->OnShaped();
            }
            else if (this->GetChunkState() == EChunkState::SurfaceReplaced)
            {
                this->OnSurfaceReplaced();
            }
            else if (this->GetChunkState() == EChunkState::ShapedCaves)
            {
                this->OnShapedCaves();
            }
            else if (this->GetChunkState() == EChunkState::Active)
            {
                this->OnActive();
            }
            else if (this->GetChunkState() == EChunkState::PendingKill)
            {
                this->OnPendingKill();
            }
            else if (this->GetChunkState() == EChunkState::Kill)
            {
                this->OnKill();
            }
            else if (this->GetChunkState() == EChunkState::BlockedByHyperlane)
            {
                this->OnBlockedByHyperlane();
            }
        }
    ));

    return;
}

bool ACommonChunk::IsStateChangeValid(const EChunkState::Type NewChunkState) const
{
    switch (NewChunkState)
    {
    case EChunkState::Invalid:
    {
        return false;
    }
    case EChunkState::Freed:
    {
        return true;
    }
    case EChunkState::PreSpawned:
    {
        return this->ChunkState == EChunkState::Freed;
    }
    case EChunkState::Spawned:
    {
        return this->ChunkState == EChunkState::PreSpawned || this->ChunkState == EChunkState::BlockedByHyperlane;
    }
    case EChunkState::Shaped:
    {
        return this->ChunkState == EChunkState::Spawned;
    }
    case EChunkState::SurfaceReplaced:
    {
        return this->ChunkState == EChunkState::Shaped;
    }
    case EChunkState::ShapedCaves:
    {
        return this->ChunkState == EChunkState::SurfaceReplaced;
    }
    case EChunkState::Active:
    {
        return this->ChunkState == EChunkState::ShapedCaves;
    }
    case EChunkState::PendingKill:
    {
        return true;
    }
    case EChunkState::Kill:
    {
        return true;
    }
    case EChunkState::BlockedByHyperlane:
    {
        return this->ChunkState == EChunkState::PreSpawned;
    }
    default:
    {
        LOG_FATAL(LogChunkValidation, "Unsupported chunk state %s.", *LexToString(NewChunkState))
        return false;
    }
    }
}

void ACommonChunk::OnSpawned(void)
{
    this->InitializeCommonStuff();
}

void ACommonChunk::OnShaped(void)
{
    this->Shape();
}

void ACommonChunk::OnSurfaceReplaced(void)
{
    this->ReplaceSurface();
}

void ACommonChunk::OnShapedCaves(void)
{
    this->ShapeCaves();
}

void ACommonChunk::OnActive(void)
{
    this->RegenerateProceduralMesh();
}

void ACommonChunk::OnPendingKill(void)
{
    this->ClearProceduralMesh();
}

void ACommonChunk::OnKill(void)
{
    this->GetChunkGenerationSubsystem()->FreeMe(*this);
}

void ACommonChunk::OnBlockedByHyperlane(void) const
{
    Cast<UHyperlaneComponent>(
        GEngine->GetFirstLocalPlayerController(this->GetWorld())
            ->GetComponentByClass(UHyperlaneComponent::StaticClass())
    )->AskServerForVoxelData_ServerRPC(this->GetChunkKeyOnTheFly());

    return;
}

bool ACommonChunk::SetChunkState(const EChunkState::Type NewChunkState, const bool bForce /* = false */)
{
    if (bForce == false && this->IsStateChangeValid(NewChunkState) == false)
    {
        LOG_FATAL(
            LogChunkValidation,
            "Invalid state change from %s to %s in chunk %s.",
            *LexToString(this->ChunkState), *LexToString(NewChunkState), *this->ChunkKey.ToString()
        )
        return false;
    }

    this->ChunkState = NewChunkState;
    this->ChunkStateChangeEvent.Broadcast(NewChunkState);

    return true;
}

#pragma endregion Chunk State

#pragma region MISC

FChunkKey ACommonChunk::GetChunkKeyOnTheFly(const bool bAllowNonZeroOnMember /* = false */) const
{
    if (bAllowNonZeroOnMember == false)
    {
        /*
         * Bad habit to use this method if the chunk is already in Spawned state.
         */
        jveryRelaxedCheck( this->ChunkKey == FChunkKey::ZeroValue )
    }

    return WorldStatics::WorldToChunkKey(this->GetActorLocation());
}

#pragma endregion MISC

#pragma region Procedural Mesh

void ACommonChunk::GenerateProceduralMesh(void)
{
    this->GenerateCollisionConvexMesh();
}

void ACommonChunk::GenerateCollisionConvexMesh(void)
{
    /*
     * Calculates convex meshes down the X axis. Each mesh is one voxel in the Z and Y direction,
     * but the X direction is the whole chunk. Separated by non-convex voxels.
     */

    this->ProceduralMeshComponent->ClearCollisionConvexMeshes();

    constexpr float ConvexX { 50.0f };
    constexpr float ConvexY { 50.0f };
    constexpr float ConvexZ { 50.0f };

    TArray<FVector> SingleVoxelConvexMesh;
    SingleVoxelConvexMesh.Add(FVector( ConvexX * 2, ConvexY * 2, ConvexZ * 2 )); /* Forward  Top    Right */
    SingleVoxelConvexMesh.Add(FVector( ConvexX * 2, ConvexY * 2, 0           )); /* Forward  Bottom Right */
    SingleVoxelConvexMesh.Add(FVector( ConvexX * 2, 0,           ConvexZ * 2 )); /* Forward  Top    Left  */
    SingleVoxelConvexMesh.Add(FVector( ConvexX * 2, 0,           0           )); /* Forward  Bottom Left  */
    SingleVoxelConvexMesh.Add(FVector( 0,           0,           ConvexZ * 2 )); /* Backward Top    Left  */
    SingleVoxelConvexMesh.Add(FVector( 0,           0,           0           )); /* Backward Bottom Left  */
    SingleVoxelConvexMesh.Add(FVector( 0,           ConvexY * 2, ConvexZ * 2 )); /* Backward Top    Right */
    SingleVoxelConvexMesh.Add(FVector( 0,           ConvexY * 2, 0           )); /* Backward Bottom Right */

    TArray<TArray<FVector>> ConvexMeshes;

    for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
    {
        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            for (int X = 0; X < WorldStatics::ChunkSize; ++X)
            {
                if (this->GetRawVoxelData(FVoxelKey(X, Y, Z)) == ECommonVoxels::Air)
                {
                    continue;
                }

                int32 AddedVoxels = 0;
                for (int ColX = X + 1; ColX < WorldStatics::ChunkSize; ++ColX)
                {
                    if (this->GetRawVoxelData(FVoxelKey(ColX, Y, Z)) == ECommonVoxels::Air)
                    {
                        break;
                    }

                    ++AddedVoxels;
                }

                TArray<FVector> CurrentVoxelConvexMesh = SingleVoxelConvexMesh;

                const FVector XOffset      = FVector(AddedVoxels * (ConvexX * 2), 0, 0);
                CurrentVoxelConvexMesh[0] += XOffset;
                CurrentVoxelConvexMesh[1] += XOffset;
                CurrentVoxelConvexMesh[2] += XOffset;
                CurrentVoxelConvexMesh[3] += XOffset;

                const FVector TotalOffset  = FVector(X * (ConvexX * 2), Y * (ConvexY * 2), Z * (ConvexZ * 2));
                CurrentVoxelConvexMesh[0] += TotalOffset;
                CurrentVoxelConvexMesh[1] += TotalOffset;
                CurrentVoxelConvexMesh[2] += TotalOffset;
                CurrentVoxelConvexMesh[3] += TotalOffset;
                CurrentVoxelConvexMesh[4] += TotalOffset;
                CurrentVoxelConvexMesh[5] += TotalOffset;
                CurrentVoxelConvexMesh[6] += TotalOffset;
                CurrentVoxelConvexMesh[7] += TotalOffset;

                ConvexMeshes.Add(CurrentVoxelConvexMesh);

                X += AddedVoxels;

                continue;
            }

            continue;
        }

        continue;
    }

    this->ProceduralMeshComponent->SetCollisionConvexMeshes(ConvexMeshes);

    return;
}

void ACommonChunk::ApplyProceduralMesh(void)
{
#if WITH_EDITOR
    if (this->GetMaterialSubsystem() == nullptr)
    {
        LOG_FATAL(LogChunkMisc, "Could not get Material Subsystem.")
        return;
    }
#endif /* WITH_EDITOR */

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        this->ProceduralMeshComponent->SetMaterial(i, this->GetMaterialSubsystem()->MDynamicGroups[i]);
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
    switch (this->GetServerChunkWorldSettings()->GetWorldGenerationType())
    {
    case EWorldGenerationType::Default:
    {
        this->Shape_Default();
        break;
    }
    case EWorldGenerationType::Superflat:
    {
        this->Shape_SuperFlat();
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

void ACommonChunk::Shape_SuperFlat(void)
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

void ACommonChunk::Shape_Default(void)
{
    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        const float WorldX = this->JChunkPosition.X + X;

        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            const float WorldY = this->JChunkPosition.Y + Y;

            const float Noise_Continentalness   = this->GetServerChunkWorldSettings()->NoiseContinentalness.GetNoise(WorldX, WorldY);
            const float Density_Continentalness = NoiseSpline::GetTargetHeight(this->GetServerChunkWorldSettings()->ContinentalnessSpline, Noise_Continentalness) * 2.0f - 1.0f;

            for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;

                const float HeightInPercent = (WorldZ / this->GetServerChunkWorldSettings()->GetHighestPoint()) * this->GetServerChunkWorldSettings()->FakeHeightMultiplier;
                const float Density         = (Density_Continentalness + 1) * HeightInPercent - 1;

                this->ModifyRawVoxelData(
                    FVoxelKey(X, Y, Z),
                    this->GetServerChunkWorldSettings()->NoiseWorld.GetNoise(WorldX, WorldY, WorldZ) < Density
                        ? ECommonVoxels::Air : ECommonVoxels::GetBaseVoxel()
                );

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::ReplaceSurface(void)
{
    switch (this->GetServerChunkWorldSettings()->GetWorldGenerationType())
    {
    case EWorldGenerationType::Default:
    {
        this->ReplaceSurface_Default();
        break;
    }
    case EWorldGenerationType::Superflat:
    {
        this->ReplaceSurface_SuperFlat();
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

// ReSharper disable once CppMemberFunctionMayBeStatic
void ACommonChunk::ReplaceSurface_SuperFlat(void)
{
    return;
}

void ACommonChunk::ReplaceSurface_Default(void)
{
    constexpr int32 SurfaceVoxel    { 4 };
    constexpr int32 SubsurfaceVoxel { 3 };
    constexpr int32 BaseVoxel       { 2 };

    constexpr int32 MaxModified { 3 };

    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            int32 Modified = 0;

            voxel_t MostTopVoxel;
            if (this->GetVoxelByNonZeroOrigin_Auth(FVoxelKey(X, Y, WorldStatics::ChunkSize), MostTopVoxel) == false)
            {
                MostTopVoxel = ECommonVoxels::Air;
            }

            if (MostTopVoxel == BaseVoxel)
            {
                Modified = MaxModified;
            }

            else if (MostTopVoxel == SurfaceVoxel)
            {
                Modified = 1;
            }

            /* This is kinda sketchy because now there may be parts with more subsurface blocks. */
            else if (MostTopVoxel == SubsurfaceVoxel)
            {
                Modified = 2;
            }

            for (int Z = WorldStatics::ChunkSize -1; Z > -1; --Z)
            {
                if (this->GetRawVoxelData(FVoxelKey(X, Y, Z)) == ECommonVoxels::Air)
                {
                    Modified = 0;
                    continue;
                }

                if (Modified >= MaxModified)
                {
                    continue;
                }

                this->ModifyRawVoxelData(FIntVector(X, Y, Z), Modified++ == 0 ? SurfaceVoxel : SubsurfaceVoxel);

                continue;
            }

            continue;
        }

        continue;
    }
}

void ACommonChunk::ShapeCaves(void)
{
    switch (this->GetServerChunkWorldSettings()->GetWorldGenerationType())
    {
    case EWorldGenerationType::Default:
    {
        this->ShapeCaves_Default();
        break;
    }
    case EWorldGenerationType::Superflat:
    {
        this->ShapeCaves_SuperFlat();
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

// ReSharper disable once CppMemberFunctionMayBeStatic
void ACommonChunk::ShapeCaves_SuperFlat(void)
{
    return;
}

void ACommonChunk::ShapeCaves_Default(void)
{
    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        const float WorldX = this->JChunkPosition.X + X;

        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            const float WorldY = this->JChunkPosition.Y + Y;

            for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
            {
                if (this->GetServerChunkWorldSettings()->NoiseCheeseCave.GetNoise(
                    WorldX,
                    WorldY,
                    static_cast<float>(this->JChunkPosition.Z + Z)
                ) < -0.5f)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), ECommonVoxels::Air);
                }

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::ModifySingleVoxel(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel)
{
    LOG_VERY_VERBOSE(
        LogChunkManipulation,
        "Requested to modify single voxel at %s to %d in %s.",
        *LocalVoxelKey.ToString(), NewVoxel, *this->ChunkKey.ToString()
    )

    if (UNetStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only server-like instances can modify voxels.")
        return;
    }

    FVoxelKey TransformedLocalVoxelLocation = LocalVoxelKey;
    ACommonChunk* TargetChunk = this->GetChunkByNonZeroOrigin_Auth(LocalVoxelKey, TransformedLocalVoxelLocation);

    if (TargetChunk == nullptr)
    {
        LOG_ERROR(
            LogChunkManipulation,
            "Could not find chunk for local voxel key %s (pivot: %s). Modified to %s.",
            *LocalVoxelKey.ToString(), *this->ChunkKey.ToString(), *TransformedLocalVoxelLocation.ToString()
        )
        return;
    }

    if (TargetChunk->GetRawVoxelData(TransformedLocalVoxelLocation) == NewVoxel)
    {
        LOG_WARNING(
            LogChunkManipulation,
            "Requested to modify single voxel at %s to %d in %s, but value is the same. Ignoring.",
            *TransformedLocalVoxelLocation.ToString(), NewVoxel, *TargetChunk->ChunkKey.ToString()
        )
        return;
    }

    TargetChunk->ModifyRawVoxelData(TransformedLocalVoxelLocation, NewVoxel);
    this->GetChunkMulticasterInfo()->BroadcastChunkModification_NetMulticastRPC(TargetChunk->ChunkKey, TransformedLocalVoxelLocation, NewVoxel);
    TargetChunk->RegenerateProceduralMesh();

    return;
}

void ACommonChunk::PredictSingleVoxelModification(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel)
{
    LOG_VERY_VERBOSE(
        LogChunkManipulation,
        "Predicting modification for single voxel at %s to %d in %s.",
        *LocalVoxelKey.ToString(), NewVoxel, *this->ChunkKey.ToString()
    )

    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only client-like instances can predict voxels manipulation outcomes.")
        return;
    }

    FVoxelKey TransformedLocalVoxelLocation = LocalVoxelKey;
    ACommonChunk* TargetChunk = this->GetChunkByNonZeroOrigin_Client(LocalVoxelKey, TransformedLocalVoxelLocation);

    if (TargetChunk == nullptr)
    {
        LOG_ERROR(
            LogChunkManipulation,
            "Could not find chunk for local voxel key %s (pivot: %s). Modified to %s.",
            *LocalVoxelKey.ToString(), *this->ChunkKey.ToString(), *TransformedLocalVoxelLocation.ToString()
        )
        return;
    }

    if (TargetChunk->GetRawVoxelData(TransformedLocalVoxelLocation) == NewVoxel)
    {
        LOG_WARNING(
            LogChunkManipulation,
            "Requested to predict modification of single voxel at %s to %d in %s, but value is the same. Ignoring.",
            *TransformedLocalVoxelLocation.ToString(), NewVoxel, *TargetChunk->ChunkKey.ToString()
        )
        return;
    }

    TargetChunk->ModifyRawVoxelData(TransformedLocalVoxelLocation, NewVoxel);
    TargetChunk->RegenerateProceduralMesh();

    return;
}

void ACommonChunk::ModifySingleVoxelOnClient(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel)
{
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Disallowed call on non server-like instance.")
        return;
    }

    if (this->GetRawVoxelData(LocalVoxelKey) == NewVoxel)
    {
        LOG_VERY_VERBOSE(
            LogChunkManipulation,
            "Requested to modify single voxel at %s to %d in %s, but value is the same. Ignoring.",
            *LocalVoxelKey.ToString(), NewVoxel, *this->ChunkKey.ToString()
        )
        return;
    }

    this->ModifyRawVoxelData(LocalVoxelKey, NewVoxel);
    this->RegenerateProceduralMesh();

    return;
}

void ACommonChunk::SetInitializationDataFromAuthority(voxel_t* Voxels)
{
#if !UE_BUILD_SHIPPING
    if (this->ChunkState != EChunkState::BlockedByHyperlane)
    {
        LOG_FATAL(
            LogChunkValidation,
            "Chunk %s is not in BlockedByHyperlane state. Found: %s.",
            *this->ChunkKey.ToString(),
            *LexToString(this->ChunkState)
        )
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    this->SetChunkState(EChunkState::Spawned);

    if (this->RawVoxelData != nullptr)
    {
        delete[] this->RawVoxelData;
    }
    this->RawVoxelData = Voxels;

    this->SetChunkState(EChunkState::Active, true);

    return;
}

ACommonChunk* ACommonChunk::GetChunkByNonZeroOrigin_Implementation(const FVoxelKey& LocalVoxelKey, FVoxelKey& OutTransformedLocalVoxelKey)
{
    FChunkKey TransformedChunkKey = this->ChunkKey;
    OutTransformedLocalVoxelKey   = LocalVoxelKey;

    if (LocalVoxelKey.X >= WorldStatics::ChunkSize)
    {
        TransformedChunkKey.X++;
        OutTransformedLocalVoxelKey.X -= WorldStatics::ChunkSize;
    }

    else if (LocalVoxelKey.X < 0)
    {
        TransformedChunkKey.X--;
        OutTransformedLocalVoxelKey.X += WorldStatics::ChunkSize;
    }

    if (LocalVoxelKey.Y >= WorldStatics::ChunkSize)
    {
        TransformedChunkKey.Y++;
        OutTransformedLocalVoxelKey.Y -= WorldStatics::ChunkSize;
    }

    else if (LocalVoxelKey.Y < 0)
    {
        TransformedChunkKey.Y--;
        OutTransformedLocalVoxelKey.Y += WorldStatics::ChunkSize;
    }

    if (LocalVoxelKey.Z >= WorldStatics::ChunkSize)
    {
        TransformedChunkKey.Z++;
        OutTransformedLocalVoxelKey.Z -= WorldStatics::ChunkSize;
    }

    else if (LocalVoxelKey.Z < 0)
    {
        TransformedChunkKey.Z--;
        OutTransformedLocalVoxelKey.Z += WorldStatics::ChunkSize;
    }

    if (OutTransformedLocalVoxelKey == LocalVoxelKey)
    {
        return this;
    }

#if LOG_PERFORMANCE_CRITICAL_SECTIONS
    LOG_VERY_VERBOSE(
        LogChunkMisc,
        "Transformed local voxel position from %s to %s. Key change from %s to %s.",
        *LocalVoxelKey.ToString(), *OutTransformedLocalVoxelKey.ToString(), *this->ChunkKey.ToString(), *TransformedChunkKey.ToString()
    )
#endif /* LOG_PERFORMANCE_CRITICAL_SECTIONS */

    ACommonChunk* TargetChunk = this->GetChunkGenerationSubsystem()->FindChunkByKey(TransformedChunkKey);

    if (TargetChunk == nullptr)
    {
        return nullptr;
    }

    const FIntVector Temp = OutTransformedLocalVoxelKey;
    return TargetChunk->GetChunkByNonZeroOrigin_Implementation(Temp, OutTransformedLocalVoxelKey);
}

ACommonChunk* ACommonChunk::GetChunkByNonZeroOrigin_Auth(const FVoxelKey& LocalVoxelKey,  FVoxelKey& OutTransformedLocalVoxelKey)
{
    if (UNetStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Disallowed call on non server-like instance.")
        return nullptr;
    }

    return this->GetChunkByNonZeroOrigin_Implementation(LocalVoxelKey, OutTransformedLocalVoxelKey);
}

ACommonChunk* ACommonChunk::GetChunkByNonZeroOrigin_Client(const FVoxelKey& LocalVoxelKey, FVoxelKey& OutTransformedLocalVoxelKey)
{
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Disallowed call on non client-like instance.")
        return nullptr;
    }

    return this->GetChunkByNonZeroOrigin_Implementation(LocalVoxelKey, OutTransformedLocalVoxelKey);
}

bool ACommonChunk::GetVoxelByNonZeroOrigin_Auth(const FVoxelKey& InLocalVoxelKey, voxel_t& OutVoxel)
{
    FVoxelKey TransformedLocalVoxelKey;
    if (const ACommonChunk* TargetChunk = this->GetChunkByNonZeroOrigin_Auth(InLocalVoxelKey, TransformedLocalVoxelKey); TargetChunk)
    {
        OutVoxel = TargetChunk->GetRawVoxelData(TransformedLocalVoxelKey);
        return true;
    }

    return false;
}

#pragma endregion Chunk World Generation
