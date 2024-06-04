// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateStatics.h"
#include "Subsystems/EngineSubsystem.h"

#include "DefaultColorsSubsystem.generated.h"

/** A subsystem that the widgets can use to get their colors. */
UCLASS(Abstract, NotBlueprintable)
class COMMONJAFGSLATE_API UDefaultColorsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UDefaultColorsSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    auto GetColorByScheme(const EJAFGColorScheme::Type InScheme) const -> FColor;
    auto GetBlurStrengthByScheme(const EJAFGBlurriness::Type InScheme) const -> float;
    auto GetFontSizeByScheme(const EJAFGFontSize::Type InScheme) const -> int32;

protected:

    /** Give ourselves to the settings so that they can update the values accordingly. */
    virtual void GiveSelfToSettings(void) PURE_VIRTUAL(UDefaultColorsSubsystem::GiveSelfToSettings)

public:

    FColor PrimaryColor          = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaMax  = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaMid  = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaLess = FColor(0, 0, 0, 0);
    FColor SecondaryColor        = FColor(0, 0, 0, 0);
    FColor InGameOSDColor        = FColor(0, 0, 0, 0);

    float  HeavyBlurStrength     = 0.0f;
    float  MediumBlurStrength    = 0.0f;
    float  LightBlurStrength     = 0.0f;

    int32  GargantuanHeaderFontSize = 0;
    int32  HeaderFontSize           = 0;
    int32  SubHeaderFontSize        = 0;
    int32  BodyFontSize             = 0;
    int32  SmallFontSize            = 0;
    int32  TinyFontSize             = 0;
};
