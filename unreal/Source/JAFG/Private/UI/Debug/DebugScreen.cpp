// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Debug/DebugScreen.h"

#include "World/WorldCharacter.h"
#include "World/Chunk/CommonChunk.h"
#include "World/Chunk/ChunkWorldSubsystem.h"

#define SANITIZED_FLT(Float, Precision)                                     \
    FString::SanitizeFloat(Float, Precision)                                ;
#define FORMAT_SANITIZED_FLT(InString, Precision)                           \
    InString.RemoveAt(                                                      \
        InString.Find(TEXT(".")) + Precision + 1,                           \
        InString.Len() - InString.Find(TEXT(".")) + Precision + 1           \
    )                                                                       ;
#define OWNING_CHARACTER                                                    \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())
#define CHECK_OWNING_CHARACTER                                              \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) )
#define CHECKED_OWNING_CHARACTER                                            \
    CHECK_OWNING_CHARACTER OWNING_CHARACTER

void UDebugScreen::Toggle(void)
{
    if (this->GetVisibility() == ESlateVisibility::Visible)
    {
        LOG_VERBOSE(LogCommonSlate, "Hiding debug screen.")
        this->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        LOG_VERBOSE(LogCommonSlate, "Showing debug screen.")
        this->SetVisibility(ESlateVisibility::Visible);
    }

    return;
}

FString UDebugScreen::GetSectionFPS(void) const
{
    FString FPS = FString::FromInt(FMath::FloorToInt(UDebugScreen::GAverageFPS()));

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
        *FString::SanitizeFloat(UDebugScreen::GetMaxFPS())
    );
}

FString UDebugScreen::GetSectionClientCharacterLocation(void) const
{
    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        FString X =
            SANITIZED_FLT(Character->GetActorLocation().X * ChunkWorldSettings::UToJScale, 3)
            FORMAT_SANITIZED_FLT(X, 3)
        FString Y =
            SANITIZED_FLT(Character->GetActorLocation().Y * ChunkWorldSettings::UToJScale, 3)
            FORMAT_SANITIZED_FLT(Y, 3)
        FString Z =
            SANITIZED_FLT(Character->GetActorLocation().Z * ChunkWorldSettings::UToJScale, 3)
            FORMAT_SANITIZED_FLT(Z, 3)

        return FString::Printf(TEXT("XYZ: %s / %s / %s"), *X, *Y, *Z);
    }

    return FString::Printf(TEXT("XYZ: N/A"));
}

FString UDebugScreen::GetSectionClientCharacterChunkLocation(void) const
{
    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        const FIntVector ChunkPosition = ACommonChunk::WorldToChunkKey(Character->GetActorLocation());
        return FString::Printf(TEXT("Chunk: %d %d %d"), ChunkPosition.X, ChunkPosition.Y, ChunkPosition.Z);
    }

    return FString::Printf(TEXT("Chunk: N/A"));
}

FString UDebugScreen::GetSectionClientCharacterFacing(void) const
{
    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        const float Yaw   = Character->GetFPSCamera()->GetComponentRotation().Yaw;
        const float Pitch = Character->GetFPSCamera()->GetComponentRotation().Pitch;

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

    return FString::Printf(TEXT("Facing: N/A"));
}

FString UDebugScreen::GetSectionTargetVoxelData(void) const
{
    if (const AWorldCharacter* Character = OWNING_CHARACTER)
    {
        ACommonChunk*               TargetedChunk;
        FVector                     TargetedWorldHitLocation;
        FVector_NetQuantizeNormal   TargetedWorldNormalHitLocation;
        FIntVector                  TargetedLocalHitVoxelLocation;
        Character->GetTargetedVoxel(
            TargetedChunk,
            TargetedWorldHitLocation,
            TargetedWorldNormalHitLocation,
            TargetedLocalHitVoxelLocation,
            false
        );

        if (TargetedChunk == nullptr)
        {
            return FString::Printf(TEXT("Targeted Voxel: N/A\n"));
        }

        FString CurrentlyMined =
            SANITIZED_FLT(Character->GetCurrentDurationSameVoxelIsMined(), 2)
            FORMAT_SANITIZED_FLT(CurrentlyMined, 2)

        return FString::Printf(TEXT("Targeted Voxel: %s\n Mined: %s / %s\n"),
            *TargetedLocalHitVoxelLocation.ToString(),
            *CurrentlyMined,
            *FString::SanitizeFloat(0.50f)
        );
    }

    return FString::Printf(TEXT("Targeted Voxel: N/A\n"));
}

#undef SANITIZED_FLT
#undef FORMAT_SANITIZED_FLT
#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
