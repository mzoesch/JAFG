// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "UsesJAFGColorScheme.h"
#include "JAFGSlateStatics.h"

#include "JAFGBorder.generated.h"

/** A border that works hand in hand with the default colors subsystem. */
UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGBorder : public UBorder, public IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    explicit UJAFGBorder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // IUsesJAFGColorScheme implementation
    virtual void UpdateComponentWithTheirScheme(void) override;
    // ~IUsesJAFGColorScheme implementation

    /**
     * Add a temporary color on top of the current color inside the brush.
     * Valid until the next update of the color scheme.
     * Either by directly calling either UpdateComponentWithTheirScheme or ResetToColorScheme or by a global
     * color scheme update.
     */
    void AddTemporarilyColor(const FColor Color);

    /**
     * Set a temporary color as the current color inside the brush.
     * Valid until the next update of the color scheme.
     * Either by directly calling either UpdateComponentWithTheirScheme or ResetToColorScheme or by a global
     * color scheme update.
     */
    void SetTemporarilyColor(const FColor Color, const bool bKeepCurrentAlpha = false);

    /** This widget must not have the value EJAFGColorScheme::DontCare set to work. */
    void ResetToColorScheme(void);

    FORCEINLINE void SetColorScheme(const EJAFGColorScheme::Type NewColorScheme) { this->ColorScheme = NewColorScheme; }

    FORCEINLINE auto LockColor(void) -> void { this->bLockedColor = true; }
    FORCEINLINE auto UnlockColor(void) -> void { this->bLockedColor = false; }
    FORCEINLINE auto IsColorLocked(void) const -> bool { return this->bLockedColor; }

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGColorScheme::Type> ColorScheme = EJAFGColorScheme::DontCare;

    /**
     * If true, the color will not be updated by a *global* color scheme update.
     * @note When calling ResetToColorScheme, the color will be updated and the lock will be removed.
     */
    bool bLockedColor = false;
};
