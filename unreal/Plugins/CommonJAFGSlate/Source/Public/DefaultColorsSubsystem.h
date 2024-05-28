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

    FColor GetColorByScheme(const EJAFGColorScheme::Type InScheme) const;

protected:

    /** Give ourselves to the settings so that they can update the values accordingly. */
    virtual void GiveSelfToSettings(void) PURE_VIRTUAL(UDefaultColorsSubsystem::GiveSelfToSettings);

public:

    FColor PrimaryColor          = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaMax  = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaMid  = FColor(0, 0, 0, 0);
    FColor PrimaryColorAlphaLess = FColor(0, 0, 0, 0);
    FColor SecondaryColor        = FColor(0, 0, 0, 0);
};
