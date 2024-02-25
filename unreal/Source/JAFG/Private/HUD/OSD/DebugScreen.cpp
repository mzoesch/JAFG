// © 2023 mzoesch. All rights reserved.

#include "HUD/OSD/DebugScreen.h"

#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#include "Core/CH_Master.h"
#include "World/ChunkWorld.h"
#include "World/JCoordinate.h"

#define SANITIZED_FLT(Float, Precision)                             \
    FString::SanitizeFloat(Float, Precision)
#define FORMAT_SANITIZED_FLT(InString, Precision)                   \
    InString.RemoveAt(                                              \
        InString.Find(TEXT(".")) + Precision + 1,                   \
        InString.Len() - InString.Find(TEXT(".")) + Precision + 1   \
    )                                                               ;

#pragma region Debug Screen Sections

FString UW_DebugScreen::GetSectionFPS() const
{
    FString FPS = FString::FromInt(FMath::FloorToInt(UW_DebugScreen::GAverageFPS()));
    if (FPS.Len() == 1)
    {
        FPS.InsertAt(0, TEXT("00"));
    }
    else if (FPS.Len() == 2)
    {
        FPS.InsertAt(0, TEXT("0"));
    }
    FString DeltaFormatted = SANITIZED_FLT(this->GetWorld()->GetDeltaSeconds() * 1000, 2); FORMAT_SANITIZED_FLT(DeltaFormatted, 2)
    if (DeltaFormatted.Find(TEXT(".")) == 1)
    {
        DeltaFormatted.InsertAt(0, TEXT("0"));
    }

    return FString::Printf(
        TEXT("%s fps @ %s ms T: %s GPU: N/A"),
        *FPS,
        *DeltaFormatted,
        *FString::SanitizeFloat(UW_DebugScreen::GetMaxFPS())
    );
}

FString UW_DebugScreen::GetSectionClientCharacterLocation() const
{
    if (const ACH_Master* Character = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        FString X   = SANITIZED_FLT(Character->GetActorLocation().X * UJCoordinate::U_TO_J_SCALE, 3); FORMAT_SANITIZED_FLT(X, 3)
        FString Y   = SANITIZED_FLT(Character->GetActorLocation().Y * UJCoordinate::U_TO_J_SCALE, 3); FORMAT_SANITIZED_FLT(Y, 3)
        FString Z   = SANITIZED_FLT(Character->GetActorLocation().Z * UJCoordinate::U_TO_J_SCALE, 3); FORMAT_SANITIZED_FLT(Z, 3)
        
        return FString::Printf(TEXT("XYZ: %s / %s / %s"), *X, *Y, *Z);
    }

    return FString::Printf(TEXT("XYZ: N/A"));
}

FString UW_DebugScreen::GetSectionClientCharacterChunkLocation() const
{
    if (const ACH_Master* Character = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        const FIntVector ChunkPosition = AChunkWorld::WorldToChunkPosition(Character->GetActorLocation());
        return FString::Printf(TEXT("Chunk: %d %d %d"), ChunkPosition.X, ChunkPosition.Y, ChunkPosition.Z);
    }

    return FString::Printf(TEXT("Chunk: N/A"));
}

FString UW_DebugScreen::GetSectionClientCharacterFacing() const
{
    if (const ACH_Master* Character = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        const float Yaw     = Character->GetFPSCamera()->GetComponentRotation().Yaw;
        const float Pitch   = Character->GetFPSCamera()->GetComponentRotation().Pitch;
	
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

        FString YawFloat    = SANITIZED_FLT(Yaw,    2); FORMAT_SANITIZED_FLT(YawFloat,   2)
        FString PitchFloat  = SANITIZED_FLT(Pitch,  2); FORMAT_SANITIZED_FLT(PitchFloat, 2)
        
        return FString::Printf(TEXT("Facing: %s (%s / %s)"), *YawAsText, *YawFloat, *PitchFloat);
    }

    return FString::Printf(TEXT("Facing: N/A"));
}

FString UW_DebugScreen::GetSectionTargetVoxelData() const
{
    if (const ACH_Master* Character = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        AChunk*                     TargetedChunk;
        FVector                     TargetedWorldLocation;
        FVector_NetQuantizeNormal   TargetedNormal;
        FIntVector                  TargetedVoxel;
        Character->GetTargetedVoxel(TargetedChunk, TargetedWorldLocation, TargetedNormal, TargetedVoxel);

        if (TargetedChunk == nullptr)
        {
            return FString::Printf(TEXT("Targeted Voxel: N/A\n"));
        }
        
        FString CurrentlyMined = SANITIZED_FLT(Character->GetCurrentDurationSameVoxelIsMined(), 2); FORMAT_SANITIZED_FLT(CurrentlyMined, 2)

        return FString::Printf(TEXT("Targeted Voxel: %s\n Mined: %s / %s\n"),
            *TargetedVoxel.ToString(),
            *CurrentlyMined,
            *FString::SanitizeFloat(0.50f)
        );
    }
    
    return FString::Printf(TEXT("Targeted Voxel: N/A\n"));
}

#pragma endregion Debug Screen Sections

#undef SANITIZED_FLT
#undef FORMAT_SANITIZED_FLT
