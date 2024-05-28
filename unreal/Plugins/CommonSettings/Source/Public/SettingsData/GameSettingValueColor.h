// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValue.h"
#include "GameSettingDataSource.h"
#include "GameSettingFilterState.h"

#include "GameSettingValueColor.generated.h"

UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UGameSettingValueColor : public UGameSettingValue
{
    GENERATED_BODY()

public:

    explicit UGameSettingValueColor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UGameSettingValue implementation
    virtual void StoreInitial(void) override;
    virtual void ResetToDefault(void) override;
    virtual void RestoreToInitial(void) override;
    // ~UGameSettingValue implementation

    FORCEINLINE auto GetDefaultValue(void) const -> FColor { return this->DefaultValue; }
    FORCEINLINE auto SetDefaultValue(const FColor& InValue) -> void { this->DefaultValue = InValue; }

    virtual auto GetValue(void) -> FColor;
    virtual auto SetValue(const FColor InValue, const EGameSettingChangeReason::Type Reason = EGameSettingChangeReason::Change) -> void;

    FORCEINLINE auto SetValueGetter(const TSharedRef<FGameSettingDataSource>& InGetter) -> void { this->ValueGetter = InGetter; }
    FORCEINLINE auto SetValueSetter(const TSharedRef<FGameSettingDataSource>& InSetter) -> void { this->ValueSetter = InSetter; }

protected:

    FColor DefaultValue;

    TSharedPtr<FGameSettingDataSource> ValueGetter;
    TSharedPtr<FGameSettingDataSource> ValueSetter;
};
