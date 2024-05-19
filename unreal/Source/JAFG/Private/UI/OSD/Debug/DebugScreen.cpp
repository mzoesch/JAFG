// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Debug/DebugScreen.h"

#include "Player/WorldPlayerController.h"
#include "WorldCore/WorldCharacter.h"
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
    }
    else
    {
        LOG_VERY_VERBOSE(LogCommonSlate, "Debug Screen is now hidden.")
        this->SetVisibility(ESlateVisibility::Collapsed);
    }
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
        TEXT("%s fps @ %s ms T: %s GPU: N/A"),
        *FPS,
        *DeltaFormatted,
        *FString::SanitizeFloat(DebugScreen::GetMaxFPS())
    );
}

FString UDebugScreen::GetSectionClientCharacterLocation(void) const
{
    FVector Location = FVector::ZeroVector;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Location = Character->GetFeetLocation();
    }

#if WITH_EDITOR
    checkSlow( GEditor )
    if (GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
        Location = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
#endif /* WITH_EDITOR */

    if (Location != FVector::ZeroVector)
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

FString UDebugScreen::GetSectionClientCharacterChunkLocation(void) const
{
    FVector Location = FVector::ZeroVector;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Location = Character->GetFeetLocation();
    }

#if WITH_EDITOR
    checkSlow( GEditor )
    if (GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
        Location = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
#endif /* WITH_EDITOR */

    if (Location != FVector::ZeroVector)
    {
        const FChunkKey ChunkKey = ChunkStatics::WorldToChunkKey(Location);
        return FString::Printf(TEXT("Chunk: %d %d %d"), ChunkKey.X, ChunkKey.Y, ChunkKey.Z);
    }

    return FString::Printf(TEXT("Chunk: N/A [REASON: Unknown]"));
}

FString UDebugScreen::GetSectionClientCharacterFacing(void) const
{
/* Yaw is clamped from -180 to 180. Therefore, this value is unreachable. */
#define UNDEFINED_ROT_FLT -200.0f

    float Yaw   = UNDEFINED_ROT_FLT;
    float Pitch = UNDEFINED_ROT_FLT;

    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        Yaw   = Character->GetFPSCamera()->GetComponentRotation().Yaw;
        Pitch = Character->GetFPSCamera()->GetComponentRotation().Pitch;
    }

#if WITH_EDITOR
    checkSlow( GEditor )
    if (GEditor->IsSimulateInEditorInProgress())
    {
        checkSlow(GCurrentLevelEditingViewportClient)
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

FString UDebugScreen::GetSectionTargetVoxelData(void) const
{
    return FString::Printf(TEXT("Targeted Voxel: N/A [REASON: Unknown]"));
}

#undef SANITIZED_FLT
#undef FORMAT_SANITIZED_FLT
#undef OWNING_CHARACTER
