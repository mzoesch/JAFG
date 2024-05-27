// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "GameSettingValue.h"
#include "JAFGInputSubsystem.h"

#include "GameSettingValueKeyIn.generated.h"

struct FKeyboardOption
{
    FKeyboardOption() = default;

    FEnhancedActionKeyMapping InputMapping { };


};

UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UGameSettingValueKeyIn : public UGameSettingValue
{
    GENERATED_BODY()

public:

    explicit UGameSettingValueKeyIn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void SetInputData(UJAFGInputSubsystem* InInputSubsystem, const FString& InMappingName);

    // UGameSettingValue implementation
    virtual auto StoreInitial(void) -> void override;
    virtual auto ResetToDefault(void) -> void override;
    virtual auto RestoreToInitial(void) -> void override;
    // ~UGameSettingValue implementation

    auto GetPrimaryKeyText(void) const -> FText;
    auto GetSecondaryKeyText(void) const -> FText;

    void ChangeBinding(const bool bIsPrimary, FKey NewKey);

private:

    UPROPERTY()
    TObjectPtr<UJAFGInputSubsystem> InputSubsystem = nullptr;
    FString MappingName = L"";
};
