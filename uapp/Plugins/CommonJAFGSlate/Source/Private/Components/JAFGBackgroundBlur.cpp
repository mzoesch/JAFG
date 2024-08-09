// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGBackgroundBlur.h"

#include "DefaultColorsSubsystem.h"

UJAFGBackgroundBlur::UJAFGBackgroundBlur(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGBackgroundBlur::UpdateComponentWithTheirScheme(void)
{
    if (this->BlurScheme == EJAFGBlurriness::DontCare)
    {
        return;
    }

    this->SetBlurStrength(
        this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetBlurStrengthByScheme(this->BlurScheme)
    );

    return;
}
