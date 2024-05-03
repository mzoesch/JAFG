// Copyright 2024 mzoesch. All rights reserved.


#include "UI/WorldHUD.h"

#include "Player/WorldPlayerController.h"
#include "JAFGSlateSettings.h"
#include "UI/OSD/EscapeMenu.h"

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldHUD::BeginPlay(void)
{
    Super::BeginPlay();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayerController());

    if (WorldPlayerController == nullptr)
    {
#if WITH_EDITOR
        LOG_WARNING(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController. Discarding HUD setup.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
        return;
    }

    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();

    // Escape menu
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->EscapeMenuWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "EscapeMenuWidgetClass is not set in project settings.")
            return;
        }
        this->EscapeMenu = CreateWidget<UEscapeMenu>(this->GetWorld(), SlateSettings->EscapeMenuWidgetClass);
        this->EscapeMenu->AddToViewport();
        this->EscapeMenu->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}
