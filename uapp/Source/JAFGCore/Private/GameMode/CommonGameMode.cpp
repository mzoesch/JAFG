// Copyright 2024 mzoesch. All rights reserved.

#include "GameModes/CommonGameMode.h"

#include "JAFGLogDefs.h"
#include "Player/CommonPlayerController.h"
#if WITH_EDITOR
    #include "Editor.h"
#endif /* WITH_EDITOR */

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
            CastedNewPlayer->ShowMouseCursor_ClientRPC(true, true, this->bCanMoveCamera == false);
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
            CastedNewPlayer->ShowMouseCursor_ClientRPC(false, true, this->bCanMoveCamera == false);
        }
        else
        {
#if WITH_EDITOR
            if (GEditor && GEditor->IsSimulateInEditorInProgress())
            {
                LOG_DISPLAY(LogCommonSlate, "Detected editor simulation mode. Discarding post login error.");
                return;
            }
            LOG_ERROR(LogGameMode, "Player controller is not of type a Common Player Controller.");
#else /* WITH_EDITOR */
            LOG_FATAL(LogGameMode, "Player controller is not of type a Common Player Controller.");
#endif /* !WITH_EDITOR */
        }
    }

    return;
}
