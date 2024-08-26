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

    if (this->IsColorLocked())
    {
        return;
    }

    this->SetBrushColor(FLinearColor(
        this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetTypeByScheme(this->ColorScheme)
    ));

    return;
}

void UJAFGBorder::AddTemporarilyColor(const FColor Color)
{
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

void UJAFGBorder::SetTemporarilyColor(const FColor Color, const bool bKeepCurrentAlpha /* = false */)
{
    if (bKeepCurrentAlpha)
    {
        this->SetBrushColor(FLinearColor(
            FLinearColor(Color).R,
            FLinearColor(Color).G,
            FLinearColor(Color).B,
            this->GetBrushColor().A
        ));
    }
    else
    {
        this->SetBrushColor(FLinearColor(Color));
    }

    return;
}

void UJAFGBorder::ResetToColorScheme(void)
{
    this->UnlockColor();
    this->UpdateComponentWithTheirScheme();

    return;
}
