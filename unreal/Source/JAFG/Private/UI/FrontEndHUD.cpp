// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEndHUD.h"

#include "JAFGSlateSettings.h"
#include "JAFGWidget.h"
#include "Player/CommonPlayerController.h"

AFrontEndHUD::AFrontEndHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AFrontEndHUD::BeginPlay(void)
{
    Super::BeginPlay();

    if (Cast<ACommonPlayerController>(this->GetOwningPlayerController()) == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type ACommonPlayerController. Discarding Front End setup.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
        return;
    }

    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();

    if (SlateSettings->FrontEndWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Front End Widget Class is not set in project settings.")
        return;
    }
    this->FrontEnd = CreateWidget<UJAFGWidget>(this->GetWorld(), SlateSettings->FrontEndWidgetClass);
    this->FrontEnd->AddToViewport();

    return;
}
