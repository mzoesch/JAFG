// Copyright 2024 mzoesch. All rights reserved.

#include "GameModes/CommonGameMode.h"

#include "Definitions.h"
#include "Player/CommonPlayerController.h"

ACommonGameMode::ACommonGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PlayerControllerClass = ACommonPlayerController::StaticClass();
    return;
}

void ACommonGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (this->bStartWithCursorVisible)
    {
        if (ACommonPlayerController* CastedNewPlayer = Cast<ACommonPlayerController>(NewPlayer))
        {
            CastedNewPlayer->ShowMouseCursor_ClientRPC(true);
        }
        else
        {
#if WITH_EDITOR
            LOG_ERROR(LogGameMode, "Player controller is not of type a Common Player Controller.");
#else /* WITH_EDITOR */
            LOG_FATAL(LogGameMode, "Player controller is not of type a Common Player Controller.");
#endif /* !WITH_EDITOR */
        }
    }
    else
    {
        if (ACommonPlayerController* CastedNewPlayer = Cast<ACommonPlayerController>(NewPlayer))
        {
            CastedNewPlayer->ShowMouseCursor_ClientRPC(false);
        }
        else
        {
#if WITH_EDITOR
            LOG_ERROR(LogGameMode, "Player controller is not of type a Common Player Controller.");
#else /* WITH_EDITOR */
            LOG_FATAL(LogGameMode, "Player controller is not of type a Common Player Controller.");
#endif /* !WITH_EDITOR */
        }
    }

    return;
}
