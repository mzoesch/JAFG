// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Debug/DebugScreen.h"

#include "GeneralProjectSettings.h"
#include "ModificationSupervisorSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/JAFGBorder.h"
#include "Components/SizeBox.h"
#include "Editor/ShippedWorldCommands.h"
#include "Engine/UserInterfaceSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "GenericPlatform/GenericPlatformDriver.h"
#include "Kismet/GameplayStatics.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/WorldCharacter.h"
#include "MISC/App.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Chunk/CommonChunk.h"
#if WITH_EDITOR
    #include "LevelEditorViewport.h"
#endif /* WITH_EDITOR */

#define SANITIZED_FLT(Float, Precision)                                                     \
    FString::SanitizeFloat(Float, Precision)                                                ;
#define FORMAT_SANITIZED_FLT(InString, Precision)                                           \
    InString.RemoveAt(                                                                      \
        InString.Find(TEXT(".")) + Precision + 1,                                           \
        InString.Len() - InString.Find(TEXT(".")) + Precision + 1                           \
    )                                                                                       ;
#define OWNING_CHARACTER                                                                    \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())

UDebugScreen::UDebugScreen(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UDebugScreen::NativeConstruct(void)
{
    Super::NativeConstruct();

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        LOG_DISPLAY(LogCommonSlate, "Detected editor simulation mode. Not subscribing to observers.")
        return;
    }
#endif /* WITH_EDITOR */

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController but HUD decided to spawn this widget anyway.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
        return;
    }

    this->DebugScreenVisibilityChangedDelegateHandle = WorldPlayerController->SubscribeToDebugScreenVisibilityChanged(ADD_SLATE_VIS_DELG(UDebugScreen::OnDebugScreenVisibilityChanged));

    this->UpdateCachedSections();

    return;
}

void UDebugScreen::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    this->LastUpdateTime += InDeltaTime;

    if (this->LastUpdateTime > this->UpdateInterval)
    {
        this->UpdateCachedSections();
        this->LastUpdateTime = 0.0f;
    }

    if (UShippedWorldCommands::Get(this)->ShowChunkPreview())
    {
        if (this->SizeBox_ChunkPreview->GetVisibility() != ESlateVisibility::Visible)
        {
            this->SizeBox_ChunkPreview->SetVisibility(ESlateVisibility::Visible);
        }
        this->UpdateChunkPreviewCanvas();
    }
    else
    {
        if (this->SizeBox_ChunkPreview->GetVisibility() != ESlateVisibility::Collapsed)
        {
            this->SizeBox_ChunkPreview->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    return;
}

void UDebugScreen::NativeDestruct(void)
{
    Super::NativeDestruct();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
        return;
    }

    if (WorldPlayerController->UnSubscribeToDebugScreenVisibilityChanged(this->DebugScreenVisibilityChangedDelegateHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Debug Screen Visibility Changed event.")
    }

    return;
}

void UDebugScreen::UpdateCachedSections(void) const
{
    this->ActorCountCache            = 0;
    this->ActorCommonChunkCountCache = 0;

    TArray<AActor*> AllUWorldActors;
    UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), AllUWorldActors);

    this->ActorCountCache = AllUWorldActors.Num();
    for (const AActor* const Actor : AllUWorldActors)
    {
        if (Actor->IsA<ACommonChunk>())
        {
            ++this->ActorCommonChunkCountCache;
        }
    }

    // Section UWorld
    //////////////////////////////////////////////////////////////////////////
    {
        this->UWorldSectionCache = FString::Printf(
            TEXT("UWorld (%s): AA: %d, ACC: %d"),
            *this->GetWorld()->GetName(),
            this->ActorCountCache,
            this->ActorCommonChunkCountCache
        );
    }
}

#if WITH_EDITOR
void UDebugScreen::ToggleInSimulateMode(void)
{
    if (this->GetVisibility() == ESlateVisibility::Visible)
    {
        this->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Visible);
    }

    return;
}
#endif /* WITH_EDITOR */

void UDebugScreen::OnDebugScreenVisibilityChanged(const bool bVisible)
{
    if (bVisible)
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Debug Screen is now visible.")
        this->SetVisibility(ESlateVisibility::Visible);
        this->UpdateCachedSections();
    }
    else
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Debug Screen is now hidden.")
        this->SetVisibility(ESlateVisibility::Collapsed);
    }
}

FString UDebugScreen::GetSectionProject(void) const
{
    if (this->ProjectSectionCache.IsEmpty())
    {
        const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();
        this->ProjectSectionCache = FString::Printf(TEXT("%s %s (%s) - %s"),
            *ProjectSettings->ProjectName,
            *ProjectSettings->ProjectVersion,
            *ProjectSettings->ProjectID.ToString(),
            *ProjectSettings->CompanyName
        );
    }

    return this->ProjectSectionCache;
}

FString UDebugScreen::GetSectionProjectGamePlay(void) const
{
    const UVoxelSubsystem* const VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    return FString::Printf(TEXT("GamePlugins: %d, [%d/%d] [%d], GAIIS: %d"),
        this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>()->ModSubsystems.Num(),
        VoxelSubsystem->GetCommonVoxelNum(),
        VoxelSubsystem->GetVoxelNum(),
        VoxelSubsystem->GetItemNum(),
        GAccumulatedItemIndexStart
    );
}

FString UDebugScreen::GetSectionEngine(void) const
{
    if (this->EngineSectionCache.IsEmpty())
    {
        this->EngineSectionCache = FString::Printf(TEXT("%s (Branch: %s) to %s %s on %s"),
            *FString(FApp::GetBuildVersion()),
            *FApp::GetBranchName(),
            *FString(LexToString(FApp::GetBuildConfiguration())),
            *FString(LexToString(FApp::GetBuildTargetType())),
            *FApp::GetBuildDate()
        );

        // LOG_WARNING(LogTemp, "%s", *FString(FApp::GetEpicProductIdentifier()))
        // LOG_WARNING(LogTemp, "%s", *FString(FApp::GetName()))
        // LOG_WARNING(LogTemp, "%s", *FString(FApp::GetZenStoreProjectId()))
        // LOG_WARNING(LogTemp, "%s", *FApp::GetInstanceId().ToString())
        // LOG_WARNING(LogTemp, "%s", *FString(FApp::GetInstanceName()))
        // LOG_WARNING(LogTemp, "%s", *FString(FApp::GetSessionId().ToString()))
        // LogTemp: Warning: UDebugScreen::GetSectionProject: UnrealEngine
        // LogTemp: Warning: UDebugScreen::GetSectionProject: UnrealEditor
        // LogTemp: Warning: UDebugScreen::GetSectionProject: JAFG.74cfd77b
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 03D7688D4E7FC98C6055E585300D4846
        // LogTemp: Warning: UDebugScreen::GetSectionProject: MZOESCH-WIN-20100
        // LogTemp: Warning: UDebugScreen::GetSectionProject: ABC1F6DB441B4263F54C459754249D85

        // LOG_WARNING(LogTemp, "%f", FApp::GetFixedDeltaTime())
        // LOG_WARNING(LogTemp, "%f", FApp::GetCurrentTime())
        // LOG_WARNING(LogTemp, "%f", FApp::GetLastTime())
        // LOG_WARNING(LogTemp, "%f", FApp::GetDeltaTime())
        // LOG_WARNING(LogTemp, "%f", FApp::GetIdleTime())
        // LOG_WARNING(LogTemp, "%f", FApp::GetGameTime())
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 0.033333
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 17794707.525000
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 17794707.511298
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 0.013703
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 0.000000
        // LogTemp: Warning: UDebugScreen::GetSectionProject: 1168.417821
    }

    return this->EngineSectionCache;
}

FString UDebugScreen::GetSectionFPS(void) const
{
    FString FPS = FString::FromInt(FMath::FloorToInt(DebugScreen::GetGAverageFPS()));

    if (FPS.Len() == 1)
    {
        FPS.InsertAt(0, TEXT("00"));
    }
    else if (FPS.Len() == 2)
    {
        FPS.InsertAt(0, TEXT("0"));
    }

    FString DeltaFormatted =
        SANITIZED_FLT(this->GetWorld()->GetDeltaSeconds() * 1000, 2)
        FORMAT_SANITIZED_FLT(DeltaFormatted, 2)

    if (DeltaFormatted.Find(TEXT(".")) == 1)
    {
        DeltaFormatted.InsertAt(0, TEXT("0"));
    }

    return FString::Printf(
        TEXT("%s fps @ %s ms T: %s; vsync (%s)"),
        *FPS,
        *DeltaFormatted,
        *FString::SanitizeFloat(DebugScreen::GetMaxFPS()),
        UGameUserSettings::GetGameUserSettings()->IsVSyncEnabled() ? TEXT("Yes") : TEXT("No")
    );
}

FString UDebugScreen::GetSectionNet(void) const
{
#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        return FString::Printf(TEXT("Net: N/A ms [REASON: Editor Simulation]"));
    }
#endif /* WITH_EDITOR */

    return FString::Printf(
        TEXT("Net M(SA: %s, SANS: %s, CL:%s, SV:%s) @ ? mst, %d mslat"),
        UNetStatics::IsSafeStandalone(this) ? TEXT("Y") : TEXT("N"),
        UNetStatics::IsSafeStandaloneNoServer(this) ? TEXT("Y") : TEXT("N"),
        UNetStatics::IsSafeClient(this) ? TEXT("Y") : TEXT("N"),
        UNetStatics::IsSafeDedicatedServer(this) ? TEXT("DS")
            : UNetStatics::IsSafeListenServer(this) ? TEXT("LS")
            : UNetStatics::IsSafeServer(this) ? TEXT("S")
            : TEXT("N"),
        static_cast<int32>(this->GetOwningPlayerState<AWorldPlayerState>()->GetPingInMilliseconds())
    );
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
FString UDebugScreen::GetSectionSession(void) const
{
    return FString::Printf(TEXT("Session: %s logged in as %s"),
        FApp::GetSessionName().IsEmpty() ? TEXT("N/A") : *FApp::GetSessionName(),
        *FApp::GetSessionOwner()
    );
}

FString UDebugScreen::GetSectionUWorld(void) const
{
    return this->UWorldSectionCache;
}

FString UDebugScreen::GetSectionChunks(void) const
{
    bool bChunksMeaningful = false;

    int32 VerticalChunks      = -1;
    int32 TotalChunks         = -1;
    int32 PendingChunks       = -1;
    int32 PendingKillChunks   = -1;
    int32 PendingClientChunks = -1;
    int32 PurgeInterval       = -1;

    if (const UChunkGenerationSubsystem* const GenSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>(); GenSubsystem)
    {
        bChunksMeaningful = true;

        VerticalChunks      = GenSubsystem->GetVerticalChunks().Num();
        TotalChunks         = GenSubsystem->ChunkMap.Num();
        PendingChunks       = GenSubsystem->VerticalChunkQueue.Num();
        PendingKillChunks   = GenSubsystem->GetPendingKillVerticalChunkQueue().Num();
        PendingClientChunks = GenSubsystem->ClientQueue.Num();
        PurgeInterval       = GenSubsystem->GetCurrentPureInterval_Inverted();
    }

    return FString::Printf(
        TEXT("Chunks (%s): %d%%, D: %d, VC %d[%d], p: %d, pk: %d, pcl: %d, purge: %d"),
        bChunksMeaningful ? TEXT("Strong") : TEXT("Weak"),
        static_cast<int32>(100.0f * (static_cast<float>(this->ActorCommonChunkCountCache) / static_cast<float>(this->ActorCountCache))),
        -1,
        VerticalChunks, TotalChunks,
        PendingChunks,
        PendingKillChunks,
        PendingClientChunks,
        PurgeInterval
    );
}

FString UDebugScreen::GetSectionClientCharacterLocation(void) const
{
    if (FVector Location; this->GetMostRespectedLocalPlayerLocation(Location))
    {
        FString X =
            SANITIZED_FLT(Location.X * WorldStatics::UToJScale, 3)
            FORMAT_SANITIZED_FLT(X, 3)
        FString Y =
            SANITIZED_FLT(Location.Y * WorldStatics::UToJScale, 3)
            FORMAT_SANITIZED_FLT(Y, 3)
        FString Z =
            SANITIZED_FLT(Location.Z * WorldStatics::UToJScale, 3)
            FORMAT_SANITIZED_FLT(Z, 3)

        return FString::Printf(TEXT("XYZ: %s / %s / %s"), *X, *Y, *Z);
    }

    return FString::Printf(TEXT("XYZ: N/A [REASON: Unknown]"));
}

FString UDebugScreen::GetSectionClientCharacterVelocity(void) const
{
    bool bMeaningful  = false;
    FVector Velocity  = FVector::ZeroVector;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Velocity = Character->GetCurrentVelocity();
        bMeaningful = true;
    }

#if WITH_EDITOR
    static FVector LastTickEditorViewportSimulationVelocity = FVector::ZeroVector;

    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
        Velocity = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation() - LastTickEditorViewportSimulationVelocity;
        LastTickEditorViewportSimulationVelocity = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
        bMeaningful = true;
    }
#endif /* WITH_EDITOR */

    if (bMeaningful == false)
    {
        return FString::Printf(TEXT("Velocity: N/A [REASON: Unknown]"));
    }

    FString X =
        SANITIZED_FLT(Velocity.X * WorldStatics::UToJScale, 3)
        FORMAT_SANITIZED_FLT(X, 3)
    FString Y =
        SANITIZED_FLT(Velocity.Y * WorldStatics::UToJScale, 3)
        FORMAT_SANITIZED_FLT(Y, 3)
    FString Z =
        SANITIZED_FLT(Velocity.Z * WorldStatics::UToJScale, 3)
        FORMAT_SANITIZED_FLT(Z, 3)

    FString Speed =
        SANITIZED_FLT(Velocity.Size() * WorldStatics::UToJScale, 2)
        FORMAT_SANITIZED_FLT(Speed, 2)

    FString HorizontalSpeed =
        SANITIZED_FLT(FVector(Velocity.X, Velocity.Y, 0).Size() * WorldStatics::UToJScale, 2)
        FORMAT_SANITIZED_FLT(HorizontalSpeed, 2)

    return FString::Printf(TEXT("Vel: %s [%s] (%s / %s / %s)"), *Speed, *HorizontalSpeed, *X, *Y, *Z);
}

FString UDebugScreen::GetSectionClientCharacterChunkLocation(void) const
{
    FVector Location = FVector::ZeroVector;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Location = Character->GetFeetLocation();
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
        Location = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
#endif /* WITH_EDITOR */

    if (Location != FVector::ZeroVector)
    {
        const FChunkKey ChunkKey = WorldStatics::WorldToChunkKey(Location);
        return FString::Printf(TEXT("Chunk: %d %d %d"), ChunkKey.X, ChunkKey.Y, ChunkKey.Z);
    }

    return FString::Printf(TEXT("Chunk: N/A [REASON: Unknown]"));
}

FString UDebugScreen::GetSectionClientCharacterFacing(void) const
{
/* Yaw is clamped from -180 to 180. Therefore, this arbitrary value is unreachable. */
#define UNDEFINED_ROT_FLT -200.0f

    float Yaw   = UNDEFINED_ROT_FLT;
    float Pitch = UNDEFINED_ROT_FLT;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Yaw   = Character->GetFPSCamera()->GetComponentRotation().Yaw;
        Pitch = Character->GetFPSCamera()->GetComponentRotation().Pitch;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow( GCurrentLevelEditingViewportClient )
        Yaw   = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetRotation().Yaw;
        Pitch = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetRotation().Pitch;
    }
#endif /* WITH_EDITOR */

    if (Yaw != UNDEFINED_ROT_FLT || Pitch != UNDEFINED_ROT_FLT)
    {
        /* Yaw is clamped from -180 to 180 */
        FString YawAsText = TEXT("N/A");

        if (Yaw >= -45.f && Yaw <= 45.f)
        {
            YawAsText = TEXT("north (Towards positive X)");
        }
        else if (Yaw > 45.f && Yaw < 135.f)
        {
            YawAsText = TEXT("east (Towards positive Y)");
        }
        else if (Yaw >= 135.f || Yaw <= -135.f)
        {
            YawAsText = TEXT("south (Towards negative X)");
        }
        else if (Yaw > -135.f && Yaw < -45.f)
        {
            YawAsText = TEXT("west (Towards negative Y)");
        }

        FString YawFloat =
            SANITIZED_FLT(Yaw, 2)
            FORMAT_SANITIZED_FLT(YawFloat, 2)
        FString PitchFloat =
            SANITIZED_FLT(Pitch, 2)
            FORMAT_SANITIZED_FLT(PitchFloat, 2)

        return FString::Printf(TEXT("Facing: %s (%s / %s)"), *YawAsText, *YawFloat, *PitchFloat);
    }

    return FString::Printf(TEXT("Facing: NA (NA / NA)"));
#undef UNDEFINED_ROT_FLT
}

FString UDebugScreen::GetSectionGenerationMisc(void) const
{
    return FString();
}

FString UDebugScreen::GetSectionMultiNoiseDebugInfo(void) const
{
    const UServerChunkWorldSettings* ChunkSettings = this->GetWorld()->GetSubsystem<UServerChunkWorldSettings>();

    FVector Location;

    if (this->GetMostRespectedLocalPlayerLocation(Location) == false || ChunkSettings == nullptr)
    {
        return FString::Printf(
            TEXT("NoiseRouter: N/A [REASON: %s]"),
            WorldStatics::IsInGameWorld(this)
                ? TEXT("Not L_World")
                : UNetStatics::IsSafeServer(this)
                    ? TEXT("Unknown")
                    : TEXT("Not Auth")
        );
    }

    Location *= WorldStatics::UToJScale;

    return FString::Printf(
        TEXT("NoiseRouter: W: %f C: %f"),
        ChunkSettings->NoiseWorld.GetNoise(Location),
        ChunkSettings->NoiseCheeseCave.GetNoise(Location)
    );
}

FString UDebugScreen::GetSectionMultiNoiseDebugInfo2() const
{
    const UServerChunkWorldSettings* ChunkSettings = this->GetWorld()->GetSubsystem<UServerChunkWorldSettings>();

    FVector2D Location;

    if (this->GetMostRespectedLocalPlayerLocation(Location) == false || ChunkSettings == nullptr)
    {
        return FString::Printf(
            TEXT("NoiseRouter2: N/A [REASON: %s]"),
            WorldStatics::IsInGameWorld(this)
                ? TEXT("Not L_World")
                : UNetStatics::IsSafeServer(this)
                    ? TEXT("Unknown")
                    : TEXT("Not Auth")
        );
    }

    Location *= WorldStatics::UToJScale;

    return FString::Printf(
        TEXT("NoiseRouter2: C: %f"),
        ChunkSettings->NoiseContinentalness.GetNoise(Location)
    );
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
FString UDebugScreen::GetSectionRAMMisc(void) const
{
    const double UsedDouble  = static_cast<double>(FPlatformMemory::GetStats().UsedPhysical);
    const double TotalDouble = static_cast<double>(FPlatformMemory::GetStats().TotalPhysical);
    const double PeakDouble  = static_cast<double>(FPlatformMemory::GetStats().PeakUsedPhysical);

    FString UtilizationFormatted =
        SANITIZED_FLT(100.0 * (UsedDouble / TotalDouble), 2)
        FORMAT_SANITIZED_FLT(UtilizationFormatted, 2)

    return FString::Printf(TEXT("Mem: %s%% %s/%sMB (P : %sMB)"),
        *UtilizationFormatted,
        *FString::Printf(TEXT("%d"), static_cast<int32>(UsedDouble  / 1024.0 /* KB */ / 1024.0 /* MB */)),
        *FString::Printf(TEXT("%d"), static_cast<int32>(TotalDouble / 1024.0 /* KB */ / 1024.0 /* MB */)),
        *FString::Printf(TEXT("%d"), static_cast<int32>(PeakDouble  / 1024.0 /* KB */ / 1024.0 /* MB */))
    );
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
FString UDebugScreen::GetSectionCPUMisc(void) const
{
    return FString::Printf(TEXT("CPU: %s(%s)[%d]"),
        *FPlatformMisc::GetCPUBrand().TrimEnd(),
        *FPlatformMisc::GetCPUVendor(),
        FPlatformMisc::GetCPUInfo()
    );
}

FString UDebugScreen::GetSectionGPUMisc(void) const
{
    if (this->GPUSectionCache.IsEmpty())
    {
        const FGPUDriverInfo Info = FPlatformMisc::GetGPUDriverInfo(FPlatformMisc::GetPrimaryGPUBrand());
        this->GPUSectionCache = FString::Printf(TEXT("GPU: %s from %s"),
            *Info.DeviceDescription,
            *Info.DriverDate
        );
    }

    return this->GPUSectionCache;
}

FString UDebugScreen::GetSectionDisplayMisc(void) const
{
    FVector2D ViewportSize; this->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

    FString DPIScaling =
        SANITIZED_FLT(GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y)), 2)
        FORMAT_SANITIZED_FLT(DPIScaling, 2)

    return FString::Printf(TEXT("Display: %dx%d (DPI: %s) (RHI: %s)"),
        static_cast<int32>(ViewportSize.X),
        static_cast<int32>(ViewportSize.Y),
        *DPIScaling,
        *FApp::GetGraphicsRHI()
    );
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeStatic
FString UDebugScreen::GetSectionTargetVoxelData(void) const
{
    return FString::Printf(TEXT("Targeted Voxel: N/A [REASON: Unknown]"));
}

bool UDebugScreen::GetMostRespectedLocalPlayerLocation(FVector& OutLocation) const
{
    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        OutLocation = Character->GetFeetLocation();
        return true;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
        OutLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
        return true;
    }
#endif /* WITH_EDITOR */

    return false;
}

bool UDebugScreen::GetMostRespectedLocalPlayerLocation(FVector2D& OutLocation) const
{
    if (FVector Location; this->GetMostRespectedLocalPlayerLocation(Location))
    {
        OutLocation = FVector2D(Location.X, Location.Y);
        return true;
    }

    return false;
}

void UDebugScreen::UpdateChunkPreviewCanvas(void) const
{
    check( this->SizeBox_ChunkPreview->GetHeightOverride() == this->SizeBox_ChunkPreview->GetWidthOverride() )

    UChunkGenerationSubsystem* ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();

    this->CanvasPanel_ChunkPreview->ClearChildren();
    const float SingleChunkSize = this->SizeBox_ChunkPreview->GetHeightOverride() / static_cast<float>(this->ChunkPreviewSize);

    FVector Pivot;
    if (this->GetMostRespectedLocalPlayerLocation(Pivot) == false)
    {
        return;
    }
    const FChunkKey2 VerticalChunkKey = WorldStatics::WorldToVerticalChunkKey(Pivot);

    for (int32 X = -this->ChunkPreviewSize / 2; X < this->ChunkPreviewSize / 2; ++X)
    {
        for (int32 Y = -this->ChunkPreviewSize / 2; Y < this->ChunkPreviewSize / 2; ++Y)
        {
            FColor Color = FColor::Red;
            if (ChunkGenerationSubsystem)
            {
                if (
                    const ACommonChunk* Chunk =
                        ChunkGenerationSubsystem->FindChunkByKey(FChunkKey(VerticalChunkKey.X + X, VerticalChunkKey.Y + Y, 0));
                    Chunk
                )
                {
                    if (
                        const EChunkState::Type State = Chunk->GetChunkState();
                        State == EChunkState::PreSpawned
                    )
                    {
                        Color = FColor::Purple;
                    }
                    else if (State == EChunkState::Spawned)
                    {
                        Color = FColor::Turquoise;
                    }
                    else if (State == EChunkState::Shaped)
                    {
                        Color = FColor::Emerald;
                    }
                    else if (State == EChunkState::Active)
                    {
                        Color = FColor::Green;
                    }
                    else if (State == EChunkState::PendingKill || State == EChunkState::Kill)
                    {
                        Color = FColor::Black;
                    }
                    else if (State == EChunkState::BlockedByHyperlane)
                    {
                        Color = FColor::Silver;
                    }
                    else
                    {
                        Color = FColor::Transparent;
                    }
                }
            }
            else
            {
                Color = FColor::Cyan;
            }

            UJAFGBorder* Border = WidgetTree->ConstructWidget<UJAFGBorder>(UJAFGBorder::StaticClass());
            FSlateBrushOutlineSettings OutlineSettings;
            OutlineSettings.Color        = X == 0 && Y == 0 ? FColor::White : FColor::Black;
            OutlineSettings.Width        = 1.0f;
            OutlineSettings.RoundingType = ESlateBrushRoundingType::Type::FixedRadius;
            FSlateBrush Brush;
            Brush.OutlineSettings = OutlineSettings;
            Brush.DrawAs          = ESlateBrushDrawType::Type::RoundedBox;
            Border->SetBrushColor(Color);
            Border->SetBrush(Brush);

            UCanvasPanelSlot* CSlot = this->CanvasPanel_ChunkPreview->AddChildToCanvas(Border);
            CSlot->SetSize(FVector2D(SingleChunkSize, SingleChunkSize));
            CSlot->SetPosition(FVector2D(
                SingleChunkSize * (Y + this->ChunkPreviewSize / 2),
                SingleChunkSize * (this->ChunkPreviewSize - (X + this->ChunkPreviewSize / 2))
            ));
        }

        continue;
    }

    return;
}

#undef SANITIZED_FLT
#undef FORMAT_SANITIZED_FLT
#undef OWNING_CHARACTER
