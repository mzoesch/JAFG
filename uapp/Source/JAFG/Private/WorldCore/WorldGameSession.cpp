// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/WorldGameSession.h"

#include "Player/WorldPlayerController.h"

AWorldGameSession::AWorldGameSession(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
}

bool AWorldGameSession::KickPlayer(APlayerController* KickedPlayer, const FText& KickReason)
{
    return this->KickPlayer(Cast<AWorldPlayerController>(KickedPlayer), KickReason);
}

bool AWorldGameSession::KickPlayer(AWorldPlayerController* KickedPlayer, const FText& KickReason)
{
    LOG_WARNING(LogWorldGameMode, "Kicking player [%s] with reason: %s", *KickedPlayer->GetDisplayName(), *KickReason.ToString())
    return Super::KickPlayer(KickedPlayer, KickReason);
}

AWorldPlayerController* AWorldGameSession::GetPlayerControllerFromDisplayName(const FString& DisplayName) const
{
    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            if (AWorldPlayerController* PlayerController = Cast<AWorldPlayerController>(*It))
            {
                if (PlayerController->GetDisplayName().Equals(DisplayName))
                {
                    return PlayerController;
                }
            }
        }
    }

    return nullptr;
}
