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
