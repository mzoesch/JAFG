// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/CommonChunk.h"

#include "HyperlaneComponent.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;
    this->bNetLoadOnClient = false;

    this->ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    /* We can do this for far away chunks, maybe? */
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    this->SubscribeWithPrivateStateDelegate();

    return;
}

ACommonChunk::~ACommonChunk(void)
{
    delete[] this->RawVoxelData;
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

void ACommonChunk::InitializeCommonStuff(void)
{
#if WITH_EDITOR
    /* PIE May not always clean up correctly. */
    if (this->RawVoxelData != nullptr)
    {
        delete[] this->RawVoxelData;
    }
#endif /* WITH_EDITOR */

    this->RawVoxelData   = new voxel_t[WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize];
    this->JChunkPosition = this->GetActorLocation() * WorldStatics::UToJScale;
    this->ChunkPosition  = FJCoordinate(
        /* We have to round here to make up on some IEEE 754 floating point precision errors. */
        FMath::RoundToFloat( this->GetActorLocation().X * WorldStatics::UToJScale ),
        FMath::RoundToFloat( this->GetActorLocation().Y * WorldStatics::UToJScale ),
        FMath::RoundToFloat( this->GetActorLocation().Z * WorldStatics::UToJScale )
    );
    this->ChunkKey       = FChunkKey(this->ChunkPosition / WorldStatics::ChunkSize);

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

void ACommonChunk::KillControlled(void)
{
    this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>()->OnChunkWasKilledExternally(this->ChunkKey);
    this->Destroy();

    return;
}

#pragma region Chunk State

auto ACommonChunk::SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive) -> void
{
    this->ChunkPersistency = NewPersistency;

    if (this->ChunkPersistency == EChunkPersistency::Persistent)
    {
        return;
    }

    this->PersistentFutureCounter.Increment();
    this->PersistencyFuture = Async(
        EAsyncExecution::ThreadPool,
        [this, TimeToLive]
        {
            FPlatformProcess::Sleep(TimeToLive);

            if (this->IsValidLowLevel() == false)
            {
                return;
            }

            this->PersistentFutureCounter.Decrement();
            if (this->PersistentFutureCounter.GetValue() > 0)
            {
                return;
            }

            /* Request to kill was invoked in some manner. */
            if (this->ChunkPersistency == EChunkPersistency::Persistent)
            {
                return;
            }

            AsyncTask(ENamedThreads::GameThread, [this]
            {
                /* Request to kill was invoked in some manner. */
                if (this->ChunkPersistency == EChunkPersistency::Persistent)
                {
                    return;
                }
                LOG_DISPLAY(LogChunkMisc, "Chunk %s will kill itself due to persistency.", *this->ChunkKey.ToString())
                this->SetChunkState(EChunkState::Kill);
            });

            return;
        }
    );

    return;
}

void ACommonChunk::SubscribeWithPrivateStateDelegate(void)
{
    this->PrivateStateHandle = this->SubscribeToChunkStateChange(FChunkStateChangeSignature::FDelegate::CreateLambda(
        [this] (const EChunkState::Type NewChunkState)
        {
            switch (NewChunkState)
            {
            case EChunkState::Invalid:
            {
                checkNoEntry()
                break;
            }
            case EChunkState::PreSpawned:
            {
                break;
            }
            case EChunkState::Spawned:
            {
                this->OnSpawned();
                break;
            }
            case EChunkState::Shaped:
            {
                this->OnShaped();
                break;
            }
            case EChunkState::SurfaceReplaced:
            {
                this->OnSurfaceReplaced();
                break;
            }
            case EChunkState::Active:
            {
                this->OnActive();
                break;
            }
            case EChunkState::PendingKill:
            {
                this->OnPendingKill();
                break;
            }
            case EChunkState::Kill:
            {
                this->OnKill();
                break;
            }
            case EChunkState::BlockedByHyperlane:
            {
                this->OnBlockedByHyperlane();
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
    ));

    return;
}

bool ACommonChunk::IsStateChangeValid(const EChunkState::Type NewChunkState)
{
    switch (NewChunkState)
    {
    case EChunkState::Invalid:
    {
        return false;
    }
    case EChunkState::PreSpawned:
    {
        return this->ChunkState == EChunkState::Invalid;
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
    case EChunkState::Active:
    {
        return this->ChunkState == EChunkState::SurfaceReplaced;
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
        LOG_FATAL(LogChunkValidation, "Unsupported chunk state %s.", *EChunkState::LexToString(NewChunkState))
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

void ACommonChunk::OnActive(void)
{
    this->RegenerateProceduralMesh();
}

void ACommonChunk::OnPendingKill(void)
{
}

void ACommonChunk::OnKill(void)
{
    this->KillControlled();
}

void ACommonChunk::OnBlockedByHyperlane(void)
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
        LOG_ERROR(
            LogChunkValidation,
            "Invalid state change from %s to %s in chunk %s.",
            *EChunkState::LexToString(this->ChunkState), *EChunkState::LexToString(NewChunkState), *this->ChunkKey.ToString()
        )
        this->KillControlled();
        return false;
    }

    this->ChunkState = NewChunkState;
    this->ChunkStateChangeEvent.Broadcast(NewChunkState);

    return true;
}

#pragma endregion Chunk State

#pragma region MISC

FChunkKey ACommonChunk::GetChunkKeyOnTheFly(void) const
{
    /*
     * Bad habit to use this method if the chunk is already in Spawned state.
     */
    check( this->ChunkKey == FChunkKey::ZeroValue )

    return FChunkKey(
        FJCoordinate(
            /* We have to round here to make up on some IEEE 754 floating point precision errors. */
            FMath::RoundToFloat( this->GetActorLocation().X * WorldStatics::UToJScale ),
            FMath::RoundToFloat( this->GetActorLocation().Y * WorldStatics::UToJScale ),
            FMath::RoundToFloat( this->GetActorLocation().Z * WorldStatics::UToJScale )
        )
        /
        WorldStatics::ChunkSize
    );
}

#pragma endregion MISC

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

void ACommonChunk::SetInitializationDataFromAuthority(voxel_t* Voxels)
{
#if !UE_BUILD_SHIPPING
    if (this->ChunkState != EChunkState::BlockedByHyperlane)
    {
        LOG_FATAL(LogChunkValidation, "Chunk %s is not in BlockedByHyperlane state.", *this->ChunkKey.ToString())
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

#pragma endregion Chunk World Generation
