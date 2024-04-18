// Copyright 2024 mzoesch. All rights reserved.

#include "Debug/DebugChunkWorldSubsystem.h"

#include "Components/SplineComponent.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/CommonChunk.h"

ADebugChunkWorldSpline::ADebugChunkWorldSpline(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    this->SetRootComponent(this->SplineComponent);

    this->SplineComponent->ClearSplinePoints(false);

    this->SplineComponent->AddSplinePoint( FVector(0.0f, 0.0f,      0.0f), ESplineCoordinateSpace::Local, false );
    this->SplineComponent->AddSplinePoint( FVector(0.0f, 0.0f, 100000.0f), ESplineCoordinateSpace::Local, true  );

    this->SplineComponent->bDrawDebug = true;

    return;
}

void ADebugChunkWorldSpline::BeginPlay(void)
{
    Super::BeginPlay();
    LOG_VERY_VERBOSE(LogChunkMisc, "Called.")
    return;
}

UDebugChunkWorldSubsystem::UDebugChunkWorldSubsystem(const FObjectInitializer& ObjectInitializer) /* : Super(ObjectInitializer) */
{
    /* The Object Initializer is not necessary for this class as it is not exposed to Kismet. */

    this->bShowChunkBorders = false;

    return;
}

bool UDebugChunkWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    check( Outer )

    const bool bSuperCreate = Super::ShouldCreateSubsystem(Outer);

    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (bSuperCreate == false)
    {
        return false;
    }

    if (UNetworkStatics::IsSafeDedicatedServer(this))
    {
        LOG_DISPLAY(LogChunkMisc, "Subsystem will not be created for dedicated server.")
        return false;
    }

    if (Outer->GetWorld()->GetName() == TEXT("L_World"))
    {
        LOG_DISPLAY(LogChunkMisc, "Subsystem will be created for world %s.", *Outer->GetWorld()->GetName())
        return true;
    }

    LOG_DISPLAY(LogChunkMisc, "Subsystem will not be created for world %s.", *Outer->GetWorld()->GetName())

    return false;
}

void UDebugChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (this->ShowChunkBordersCommand)
    {
        LOG_FATAL(LogChunkMisc, "Command already exists.")
        return;
    }

    return;
}

void UDebugChunkWorldSubsystem::PostInitialize(void)
{
    Super::PostInitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UDebugChunkWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (this->ShowChunkBordersCommand)
    {
        LOG_FATAL(LogChunkMisc, "Command already exists.")
        return;
    }

    this->ShowChunkBordersCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("j.ShowChunkBorders"),
        TEXT("Visually shows the chunk borders in the UWorld."),
        FConsoleCommandDelegate::CreateLambda( [this] (void)
        {
            this->OnToggleChunkBorders();
        }),
        ECVF_Cheat
    );

    this->bShowChunkBorders = false;

    return;
}

void UDebugChunkWorldSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (this->ShowChunkBordersCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(this->ShowChunkBordersCommand);
        this->ShowChunkBordersCommand = nullptr;
    }

    return;
}

void UDebugChunkWorldSubsystem::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->bShowChunkBorders == false)
    {
        return;
    }

    this->UpdateAllDebugSplines(false);

    return;
}

void UDebugChunkWorldSubsystem::OnToggleChunkBorders(void)
{
    if (this->bShowChunkBorders)
    {
        LOG_DISPLAY(LogChunkMisc, "Hiding chunk borders.")

        this->bShowChunkBorders = false;
        this->OnHideChunkBorders();

        return;
    }

    LOG_DISPLAY(LogChunkMisc, "Showing chunk borders.")

    this->bShowChunkBorders = true;
    this->OnShowChunkBorders();

    return;
}

void UDebugChunkWorldSubsystem::OnShowChunkBorders(void)
{
    check( GEngine )
    check( this->GetWorld() )
    GEngine->Exec(this->GetWorld(), TEXT("ShowFlag.Splines 1"));

    this->UpdateAllDebugSplines(true);

    return;
}

void UDebugChunkWorldSubsystem::OnHideChunkBorders(void)
{
    check( GEngine )
    check( this->GetWorld() )
    GEngine->Exec(this->GetWorld(), TEXT("ShowFlag.Splines 0"));

    for (ADebugChunkWorldSpline* DebugChunkWorldSpline : this->DebugChunkWorldSplines)
    {
        DebugChunkWorldSpline->Destroy();
    }
    this->DebugChunkWorldSplines.Empty();

    return;
}

void UDebugChunkWorldSubsystem::UpdateAllDebugSplines(const bool bForceUpdate)
{
    check( GEngine )
    check( this->GetWorld() )
    check( GEngine->GetFirstGamePlayer(this->GetWorld()) )
    check( GEngine->GetFirstGamePlayer(this->GetWorld())->PlayerController )
    FVector LocalPlayerPosition = GEngine->GetFirstGamePlayer(this->GetWorld())->PlayerController->GetPawn()->GetActorLocation();

    FIntVector LocalPlayerChunkKey = ACommonChunk::WorldToChunkKey(LocalPlayerPosition);
    FIntVector2 LocalPlayerChunkKeyXY = FIntVector2(LocalPlayerChunkKey.X, LocalPlayerChunkKey.Y);

    if (this->LastLocalPlayerPosition == LocalPlayerChunkKeyXY && bForceUpdate == false)
    {
        return;
    }

    this->LastLocalPlayerPosition = LocalPlayerChunkKeyXY;

    for (ADebugChunkWorldSpline* DebugChunkWorldSpline : this->DebugChunkWorldSplines)
    {
        DebugChunkWorldSpline->Destroy();
    }
    this->DebugChunkWorldSplines.Empty();

    for (int i = 0; i < 4; ++i)
    {
        FTransform SplineTransform = FTransform::Identity;
        FVector Loca = FVector(
            this->LastLocalPlayerPosition.X * ChunkWorldSettings::JToUScale * ChunkWorldSettings::ChunkSize +
            (i % 2 == 0 ? 0.0f : ChunkWorldSettings::JToUScale * ChunkWorldSettings::ChunkSize),
            this->LastLocalPlayerPosition.Y * ChunkWorldSettings::JToUScale * ChunkWorldSettings::ChunkSize +
            (i < 2 ? 0.0f : ChunkWorldSettings::JToUScale * ChunkWorldSettings::ChunkSize),
            0.0f
            );
        SplineTransform.SetLocation(Loca);

        ADebugChunkWorldSpline* Spline = this->GetWorld()->SpawnActor<ADebugChunkWorldSpline>(ADebugChunkWorldSpline::StaticClass(), SplineTransform);

        this->DebugChunkWorldSplines.Add(Spline);

        continue;
    }

    return;
}
