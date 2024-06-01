// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingFilterState.h"
#include "GameSettingValue.h"
#include "GameSettingDataSource.h"

#include "GameSettingValueScalar.generated.h"

typedef TFunction<FText(const double Value)> FSettingScalarFormatFunction;

UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UGameSettingValueScalar : public UGameSettingValue
{
    GENERATED_BODY()

public:

    explicit UGameSettingValueScalar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UGameSettingValue implementation
    virtual void StoreInitial(void) override;
    virtual void ResetToDefault(void) override;
    virtual void RestoreToInitial(void) override;
    // ~UGameSettingValue implementation

    static FSettingScalarFormatFunction Raw;
    static FSettingScalarFormatFunction ZeroToOneAsPercent;

    FORCEINLINE auto SetDisplayFormat(const FSettingScalarFormatFunction& InDisplayFormat) -> void { this->DisplayFormat = InDisplayFormat; }
                auto GetFormattedText(void) const -> FText;

    FORCEINLINE         auto SetDefaultValue(const double InValue) -> void { this->DefaultValue = InValue; }
    FORCEINLINE         auto SetDefaultValue(const float  InValue) -> void { this->SetDefaultValue(static_cast<double>(InValue)); }
    FORCEINLINE virtual auto GetDefaultValue(void) const -> TOptional<double> { return this->DefaultValue; }

    virtual auto GetValue(void) const -> double;
            auto GetValueNormalized(void) -> double;
    virtual auto SetValue(double InValue, EGameSettingChangeReason::Type Reason = EGameSettingChangeReason::Change) -> void;

    FORCEINLINE auto SetValueGetter(const TSharedRef<FGameSettingDataSource>& InGetter) -> void { this->ValueGetter = InGetter; }
    FORCEINLINE auto SetValueSetter(const TSharedRef<FGameSettingDataSource>& InSetter) -> void { this->ValueSetter = InSetter; }

    FORCEINLINE auto SetUserMinimum(const double InMinimum) -> void { this->Minimum = InMinimum; }
    FORCEINLINE auto SetUserMinimum(const float  InMinimum) -> void { this->SetUserMinimum(static_cast<double>(InMinimum)); }
    FORCEINLINE auto GetUserMinimum(void) const -> TOptional<double> { return this->Minimum; }
    FORCEINLINE auto SetUserMaximum(const double InMaximum) -> void { this->Maximum = InMaximum; }
    FORCEINLINE auto SetUserMaximum(const float  InMaximum) -> void { this->SetUserMaximum(static_cast<double>(InMaximum)); }
    FORCEINLINE auto GetUserMaximum(void) const -> TOptional<double> { return this->Maximum; }

    FORCEINLINE auto SetValueStep(const double InValueStep) -> void { this->ValueStep = InValueStep; }
    FORCEINLINE auto SetValueStep(const float  InValueStep) -> void { this->SetValueStep(static_cast<double>(InValueStep)); }

    /**
     * @param Value Has to be [0, 1]. Will transform it to the local used range in this setting.
     *              Requires that UGameSettingValueScalar#Minimum and UGameSettingValueScalar#Maximum are set.
     */
    float TransformRangeToValue(const float Value);

protected:

    TOptional<double> DefaultValue;

    TSharedPtr<FGameSettingDataSource> ValueGetter;
    TSharedPtr<FGameSettingDataSource> ValueSetter;

    double InitialValue = 0;

    double ValueStep = 0.01;

    /**
     * A minimum for the user interface. Will only affect the user interface and not the actual value. If they, for
     * example, use a slider (A slider has always a range between 0 and 1, but we may want to change the range to
     * something bigger or smaller). This is not enforced.
     */
    TOptional<double> Minimum;
    /**
     * A maximum for the user interface. Will only affect the user interface and not the actual value. If they, for
     * example, use a slider (A slider has always a range between 0 and 1, but we may want to change the range to
     * something bigger or smaller). This is not enforced.
     */
    TOptional<double> Maximum;

    /**
     * @return To be meaningful, it requires UGameSettingValueScalar#Minimum and
     *         UGameSettingValueScalar#Maximum to be set.
     */
    TRange<double> GetSourceRange(void);

    FSettingScalarFormatFunction DisplayFormat;
};
