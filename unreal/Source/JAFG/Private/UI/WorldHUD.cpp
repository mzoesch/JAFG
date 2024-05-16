// Copyright 2024 mzoesch. All rights reserved.


#include "UI/WorldHUD.h"

#include "Player/WorldPlayerController.h"
#include "JAFGSlateSettings.h"
#include "UI/OSD/EscapeMenu.h"
#include "UI/OSD/Debug/DebugScreen.h"

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldHUD::BeginPlay(void)
{
    Super::BeginPlay();

    if (Cast<AWorldPlayerController>(this->GetOwningPlayerController()) == nullptr)
    {
#if WITH_EDITOR
        if (GEditor && GEditor->IsSimulateInEditorInProgress())
        {
            LOG_DISPLAY(LogCommonSlate, "Simulation mode detected. Creating simulation HUD.")
            this->CreateSimulationHUD();
            return;
        }
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController. Discarding HUD setup.")
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

    // Debug Screen
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->DebugScreenWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "DebugScreenWidgetClass is not set in project settings.")
            return;
        }
        this->DebugScreen = CreateWidget<UDebugScreen>(this->GetWorld(), SlateSettings->DebugScreenWidgetClass);
        this->DebugScreen->AddToViewport();
        this->DebugScreen->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}

void AWorldHUD::CreateSimulationHUD(void)
{
    // ReSharper disable once CppTooWideScope
    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();

    // Debug Screen
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->DebugScreenWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "DebugScreenWidgetClass is not set in project settings.")
            return;
        }
        this->DebugScreen = CreateWidget<UDebugScreen>(this->GetWorld(), SlateSettings->DebugScreenWidgetClass);
        this->DebugScreen->AddToViewport();
        this->DebugScreen->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}
