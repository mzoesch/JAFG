// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Debug/ChunkDebugSplineSubsystem.h"
#include "Components/SplineComponent.h"
#include "Editor/ShippedWorldCommands.h"
#include "WorldCore/WorldCharacter.h"
#if WITH_EDITOR
    #include "LevelEditorViewport.h"
#endif /* WITH_EDITOR */

AChunkDebugWorldSpline::AChunkDebugWorldSpline(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
    this->SetRootComponent(this->SplineComponent);

    this->SplineComponent->ClearSplinePoints(false);

    this->SplineComponent->AddSplinePoint( FVector(0.0f, 0.0f,      0.0f), ESplineCoordinateSpace::Local, false );
    this->SplineComponent->AddSplinePoint( FVector(0.0f, 0.0f, 100000.0f), ESplineCoordinateSpace::Local, true  );

    this->SplineComponent->bDrawDebug = true;

    return;
}

UChunkDebugSplineSubsystem::UChunkDebugSplineSubsystem(void)
{
    return;
}

bool UChunkDebugSplineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeDedicatedServer(this) == false;
}

void UChunkDebugSplineSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    UShippedWorldCommands* Commands = UShippedWorldCommands::Get(InWorld);

    this->OnCVarChangedHandle = Commands->AttachToConsoleVariable(CV_CHUNK_BORDERS, FConsoleVariableDelegate::CreateLambda(
    [this] (const IConsoleVariable* Variable)
    {
        if (this->GetWorld())
        {
            LOG_DISPLAY(LogChunkMisc, "Toggled to %d.", Variable->GetBool());
            this->OnToggle(Variable->GetBool());
        }
    }));

    this->OnToggle(Commands->ShowChunkPreview());

    return;
}

void UChunkDebugSplineSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    if (this->OnCVarChangedHandle.IsValid())
    {
        if (this->GetWorld() == nullptr)
        {
            LOG_FATAL(LogChunkMisc, "World is null.");
        }

        else if (
            UShippedWorldCommands* Commands = UShippedWorldCommands::Get(this->GetWorld());
            Commands->DetachFromConsoleVariable(CV_CHUNK_BORDERS, this->OnCVarChangedHandle) == false
        )
        {
            LOG_ERROR(LogChunkMisc, "Failed to detach from console variable changed event.");
        }
        this->OnCVarChangedHandle.Reset();
    }

    return;
}

void UChunkDebugSplineSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);
    this->UpdateAllSplines();
    return;
}

void UChunkDebugSplineSubsystem::OnToggle(const bool bInEnabled)
{
    this->bEnabled = bInEnabled;
    this->bEnabled ? this->OnShow() : this->OnHide();

    return;
}

void UChunkDebugSplineSubsystem::OnShow(void)
{
    check( GEngine )
    if (this->GetWorld())
    {
        GEngine->Exec(this->GetWorld(), TEXT("ShowFlag.Splines 1"));
    }

    this->UpdateAllSplines(true);

    return;
}

void UChunkDebugSplineSubsystem::OnHide(void)
{
    check( GEngine )
    if (this->GetWorld())
    {
        GEngine->Exec(this->GetWorld(), TEXT("ShowFlag.Splines 0"));
    }

    for (AChunkDebugWorldSpline* Spline : this->Splines)
    {
        Spline->Destroy();
    }

    this->Splines.Empty();

    return;
}

void UChunkDebugSplineSubsystem::UpdateAllSplines(const bool bForce /* = false */)
{
    if (this->GetWorld() == nullptr)
    {
        return;
    }

    const APlayerController* PlayerController = this->GetWorld()->GetFirstPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    FVector LocalPlayerLocation;
    if (this->GetMostRespectedLocalPlayerLocation(LocalPlayerLocation) == false)
    {
        return;
    }

    const FChunkKey2 LocalChunkKey = WorldStatics::WorldToVerticalChunkKey(LocalPlayerLocation);

    if (this->LastLocalPlayerLocation == LocalChunkKey && bForce == false)
    {
        return;
    }

    this->LastLocalPlayerLocation = LocalChunkKey;
    TArray<FChunkKey2> Chunks = Validation::GetAllChunksInDistance(this->LastLocalPlayerLocation, 2);

    int n = 0;
    for (AChunkDebugWorldSpline* Spline : this->Splines)
    {
        if (n >= Chunks.Num())
        {
            Spline->Destroy();
            continue;
        }

        Spline->SetActorLocation(
            WorldStatics::VerticalChunkKeyToWorldLocation(Chunks[n]),
            false,
            nullptr,
            ETeleportType::ResetPhysics
        );
        ++n;
    }

    for (int i = n; i < Chunks.Num(); ++i)
    {
        this->Splines.Emplace(this->GetWorld()->SpawnActor<AChunkDebugWorldSpline>(
            AChunkDebugWorldSpline::StaticClass(),
            WorldStatics::VerticalChunkKeyToWorldLocation(Chunks[i]),
            FRotator::ZeroRotator
        ));
    }

    return;
}

bool UChunkDebugSplineSubsystem::GetMostRespectedLocalPlayerLocation(FVector& OutLocation) const
{
    if (this->GetWorld() == nullptr)
    {
        return false;
    }

    if (const APlayerController* PlayerController = this->GetWorld()->GetFirstPlayerController(); PlayerController)
    {
        if (const AWorldCharacter* Character = Cast<AWorldCharacter>(PlayerController->GetCharacter()))
        {
            OutLocation = Character->GetFeetLocation();
            return true;
        }
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow( GCurrentLevelEditingViewportClient )
        OutLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
        return true;
    }
#endif /* WITH_EDITOR */

    return false;
}
