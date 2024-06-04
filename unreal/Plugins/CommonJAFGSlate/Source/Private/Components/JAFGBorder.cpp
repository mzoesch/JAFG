// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGBorder.h"

#include "DefaultColorsSubsystem.h"
#include "JAFGLogDefs.h"
#include "JAFGSlateStatics.h"

UJAFGBorder::UJAFGBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGBorder::UpdateComponentWithTheirScheme(void)
{
    if (this->ColorScheme == EJAFGColorScheme::DontCare)
    {
        return;
    }

    this->SetBrushColor(FLinearColor(
        this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetColorByScheme(this->ColorScheme)
    ));

    return;
}

void UJAFGBorder::AddTemporarilyColor(const FColor Color)
{
#if !UE_BUILD_SHIPPING
    if (this->ColorScheme == EJAFGColorScheme::DontCare)
    {
        LOG_FATAL(LogCommonSlate, "This widget must not have the value EJAFGColorScheme::DontCare set to work.")
    }
#endif /* !UE_BUILD_SHIPPING */

    const FLinearColor TempColor    = FLinearColor(Color);
    const FLinearColor CurrentColor = this->GetBrushColor();

    this->SetBrushColor(FLinearColor(
        CurrentColor.R + TempColor.R,
        CurrentColor.G + TempColor.G,
        CurrentColor.B + TempColor.B,
        CurrentColor.A + TempColor.A
    ));

    return;
}

void UJAFGBorder::ResetToColorScheme(void)
{
    this->UpdateComponentWithTheirScheme();
}
