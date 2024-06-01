// Copyright 2024 mzoesch. All rights reserved.

#include "DefaultColorsSubsystem.h"

#include "JAFGLogDefs.h"

UDefaultColorsSubsystem::UDefaultColorsSubsystem() : Super()
{
    return;
}

void UDefaultColorsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    this->GiveSelfToSettings();
    return;
}

FColor UDefaultColorsSubsystem::GetColorByScheme(const EJAFGColorScheme::Type InScheme) const
{
    switch (InScheme)
    {
    case EJAFGColorScheme::Primary:
    {
        return this->PrimaryColor;
    }
    case EJAFGColorScheme::PrimaryAlphaMax:
    {
        return this->PrimaryColorAlphaMax;
    }
    case EJAFGColorScheme::PrimaryAlphaMid:
    {
        return this->PrimaryColorAlphaMid;
    }
    case EJAFGColorScheme::PrimaryAlphaLess:
    {
        return this->PrimaryColorAlphaLess;
    }
    case EJAFGColorScheme::Secondary:
    {
        return this->SecondaryColor;
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Could not find color scheme: %d.", InScheme)
        return FColor();
    }
    }
}

float UDefaultColorsSubsystem::GetBlurStrengthByScheme(const EJAFGBlurriness::Type InScheme) const
{
    switch (InScheme)
    {
    case EJAFGBlurriness::Heavy:
    {
        return this->HeavyBlurStrength;
    }
    case EJAFGBlurriness::Medium:
    {
        return this->MediumBlurStrength;
    }
    case EJAFGBlurriness::Light:
    {
        return this->LightBlurStrength;
    }
    case EJAFGBlurriness::None:
    {
        return 0.0f;
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Could not find blurriness scheme: %d.", InScheme)
        return 0.0f;
    }
    }
}

int32 UDefaultColorsSubsystem::GetFontSizeByScheme(const EJAFGFontSize::Type InScheme) const
{
    switch (InScheme)
    {
    case EJAFGFontSize::GargantuanHeader:
    {
        return this->GargantuanHeaderFontSize;
    }
    case EJAFGFontSize::Header:
    {
        return this->HeaderFontSize;
    }
    case EJAFGFontSize::SubHeader:
    {
        return this->SubHeaderFontSize;
    }
    case EJAFGFontSize::Body:
    {
        return this->BodyFontSize;
    }
    case EJAFGFontSize::Small:
    {
        return this->SmallFontSize;
    }
    case EJAFGFontSize::Tiny:
    {
        return this->TinyFontSize;
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Could not find font size scheme: %d.", InScheme)
        return 0.0f;
    }
    }
}
