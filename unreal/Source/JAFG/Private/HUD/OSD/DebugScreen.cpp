// © 2023 mzoesch. All rights reserved.

#include "HUD/OSD/DebugScreen.h"

#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#include "Core/CH_Master.h"
#include "World/ChunkWorld.h"
#include "World/JCoordinate.h"

#pragma region Debug Screen Sections

float UW_DebugScreen::GAverageFPS()
{
    extern ENGINE_API float GAverageFPS;
    return GAverageFPS; 
}

float UW_DebugScreen::GetMaxFPS()
{
    if (const UEngine* Engine = Cast<UEngine>(GEngine))
    {
        return Engine->GetMaxFPS();
    }

    return MAX_flt;
}

FString UW_DebugScreen::GetSectionFPS() const
{
    FString DeltaFormatted = FString::SanitizeFloat(this->GetWorld()->GetDeltaSeconds() * 1000, 2);
    DeltaFormatted.RemoveAt(DeltaFormatted.Find(TEXT(".")) + 3, DeltaFormatted.Len() - (DeltaFormatted.Find(TEXT(".")) + 3));
    if (DeltaFormatted.Find(TEXT(".")) == 1)
    {
        DeltaFormatted.InsertAt(0, TEXT("0"));
    }

    return FString::Printf(
        TEXT("%s fps @ %s ms T: %s GPU: N/A"),
        *FString::FromInt(FMath::FloorToInt(UW_DebugScreen::GAverageFPS())),
        *DeltaFormatted,
        *FString::SanitizeFloat(UW_DebugScreen::GetMaxFPS())
    );
}

FString UW_DebugScreen::GetSectionClientCharacterLocation() const
{
    if (const ACH_Master* CH = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        FString X = FString::SanitizeFloat(CH->GetActorLocation().X * UJCoordinate::U_TO_J_SCALE, 3);
        X.RemoveAt(X.Find(TEXT(".")) + 4, X.Len() - X.Find(TEXT(".")) + 4);

        FString Y = FString::SanitizeFloat(CH->GetActorLocation().Y * UJCoordinate::U_TO_J_SCALE, 3);
        Y.RemoveAt(Y.Find(TEXT(".")) + 4, Y.Len() - Y.Find(TEXT(".")) + 4);
	
        FString Z = FString::SanitizeFloat(CH->GetActorLocation().Z * UJCoordinate::U_TO_J_SCALE, 3);
        Z.RemoveAt(Z.Find(TEXT(".")) + 4, Z.Len() - Z.Find(TEXT(".")) + 4);
        
        return FString::Printf(TEXT("XYZ: %s / %s / %s"), *X, *Y, *Z);
    }

    return FString::Printf(TEXT("XYZ: N/A"));
}

FString UW_DebugScreen::GetSectionClientCharacterChunkLocation() const
{
    if (const ACH_Master* CH = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        const FIntVector ChunkPosition = AChunkWorld::WorldToChunkPosition(CH->GetActorLocation());
        return FString::Printf(TEXT("Chunk: %d %d %d"), ChunkPosition.X, ChunkPosition.Y, ChunkPosition.Z);
    }

    return FString::Printf(TEXT("Chunk: N/A"));
}

FString UW_DebugScreen::GetSectionClientCharacterFacing() const
{
    if (const ACH_Master* CH = Cast<ACH_Master>(this->GetOwningPlayer()->GetCharacter()))
    {
        const float Yaw     = CH->GetFPSCamera()->GetComponentRotation().Yaw;
        const float Pitch   = CH->GetFPSCamera()->GetComponentRotation().Pitch;
	
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

        FString YawFloat = FString::SanitizeFloat(Yaw, 2);
        YawFloat.RemoveAt(YawFloat.Find(TEXT(".")) + 3, YawFloat.Len() - (YawFloat.Find(TEXT(".")) + 3));

        FString PitchFloat = FString::SanitizeFloat(Pitch, 2);
        PitchFloat.RemoveAt(PitchFloat.Find(TEXT(".")) + 3, PitchFloat.Len() - (PitchFloat.Find(TEXT(".")) + 3));
        
        return FString::Printf(TEXT("Facing: %s (%s / %s)"), *YawAsText, *YawFloat, *PitchFloat);
    }

    return FString::Printf(TEXT("Facing: N/A"));
}

#pragma endregion Debug Screen Sections
