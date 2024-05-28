// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGBorder.h"

#include "DefaultColorsSubsystem.h"
#include "JAFGSlateStatics.h"

UJAFGBorder::UJAFGBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGBorder::UpdateBrushWithSetColorScheme(void)
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
