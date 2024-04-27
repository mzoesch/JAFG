// Copyright 2024 mzoesch. All rights reserved.

#include "MenuHUD.h"

#include "JAFGFrontEnd.h"

AMenuHUD::AMenuHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AMenuHUD::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeStandalone(this) == false)
    {
        LOG_FATAL(LogCommonSlate, "Dissallowed on not standalone instances.")
        return;
    }

    LOG_VERBOSE(LogCommonSlate, "Called.")

    if (this->FrontEndWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "FrontEndWidgetClass is invalid.")
        return;
    }

    this->FrontEndWidget = CreateWidget<UJAFGFrontEnd>(this->GetWorld(), this->FrontEndWidgetClass);
    this->FrontEndWidget->AddToViewport();

    return;
}
