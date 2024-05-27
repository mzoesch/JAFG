// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingFilterState.h"
#include "GameSettingValue.h"
#include "GameSettingDataSource.h"

#include "GameSettingValueScalar.generated.h"

typedef TFunction<FText(const double Value)> FSettingScalarFormatFunction;

UCLASS()
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
    virtual auto SetValue(double InValue, EGameSettingChangeReason::Type Reason = EGameSettingChangeReason::Change) -> void;

    FORCEINLINE auto SetValueGetter(const TSharedRef<FGameSettingDataSource>& InGetter) -> void { this->ValueGetter = InGetter; }
    FORCEINLINE auto SetValueSetter(const TSharedRef<FGameSettingDataSource>& InSetter) -> void { this->ValueSetter = InSetter; }

protected:

    TOptional<double> DefaultValue;

    TSharedPtr<FGameSettingDataSource> ValueGetter;
    TSharedPtr<FGameSettingDataSource> ValueSetter;

    double InitialValue = 0;

    TRange<double> ValueRange = TRange<double>(0, 1);
    double ValueStep = 0.01;

    TOptional<double> Minimum;
    TOptional<double> Maximum;

    FSettingScalarFormatFunction DisplayFormat;
};
