// Copyright 2024 mzoesch. All rights reserved.

#include "DefaultColorsSubsystem.h"

#include "JAFGLogDefs.h"
#include "JAFGMacros.h"
#include "System/FontSubsystem.h"

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

FColor UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGColorScheme::Type InScheme) const
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
    case EJAFGColorScheme::InGameOSD:
    {
        return this->InGameOSDColor;
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Could not find color scheme: %d.", InScheme)
        return FColor();
    }
    }
}

float UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGBlurriness::Type InScheme) const
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

int32 UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGFontSize::Type InScheme) const
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

TSharedPtr<FCompositeFont> UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGFont::Type InScheme) const
{
    if (InScheme == EJAFGFont::Core)
    {
        return this->GetGameInstance()->GetSubsystem<UFontSubsystem>()->CoreFontData.ConstructCompositeFont();
    }

    checkNoEntry()

    return nullptr;
}

UDataTable* UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGRichFont::Type InScheme) const
{
    if (InScheme == EJAFGRichFont::Core)
    {
        return this->GetGameInstance()->GetSubsystem<UFontSubsystem>()->CoreFontData.ConstructNewMinimalDataTable();
    }

    jcheckNoEntry()

    return nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FName UDefaultColorsSubsystem::GetTypeByScheme(const EJAFGFontTypeFace::Type InScheme) const
{
    return UFontSubsystem::GetTypeByScheme(InScheme);
}

void UDefaultColorsSubsystem::ParseTypeByScheme(const EJAFGFont::Type InScheme, FSlateFontInfo& Info) const
{
    Info.FontObject = nullptr;

    if (InScheme == EJAFGFont::Core)
    {
        Info.CompositeFont = this->GetTypeByScheme(InScheme);
        return;
    }

    jcheckNoEntry()

    return;
}

void UDefaultColorsSubsystem::ParseTypeByScheme(const EJAFGFont::Type InScheme, const EJAFGFontTypeFace::Type InTypeFace, FSlateFontInfo& Info) const
{
    this->ParseTypeByScheme(InScheme, Info);
    Info.TypefaceFontName = UFontSubsystem::GetTypeByScheme(InTypeFace);

    return;
}
