// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "UObject/Object.h"

#include "GameSetting.generated.h"

JAFG_VOID

class USettingRegistry;

UCLASS()
class JAFGCORE_API UGameSetting : public UObject
{
    GENERATED_BODY()

public:

    explicit UGameSetting(const FObjectInitializer& ObjectInitializer = FObjectInitializer());

    /**
     * Any setting can have children. This is so we can allow for the possibility of "collections" or "actions" that
     * are not directly visible to the user, but are set by some means and need to have initial and restored values.
     * In that case, you would likely have internal settings inside an action subclass that is set on another screen,
     * but never directly listed on the settings panel.
     */
    FORCEINLINE virtual auto GetChildSettings(void) -> TArray<UGameSetting*> { return TArray<UGameSetting*>(); }

    FORCEINLINE         auto SetRegistry(USettingRegistry* InOwningRegistry) -> void { this->OwningRegistry = InOwningRegistry; }

    UFUNCTION(BlueprintPure, Category = "JAFG|Settings")
    FString GetIdentifier(/* void */) const { return this->Identifier; }
    auto    SetIdentifier(const FString& InIdentifier) -> void { this->Identifier = InIdentifier; }

    UFUNCTION(BlueprintPure, Category = "JAFG|Settings")
    FText GetSettingDisplayName(/* void */) const { return this->SettingDisplayName; }
    auto  SetSettingDisplayName(const FText& InSettingDisplayName) -> void { this->SettingDisplayName = InSettingDisplayName; }

protected:

    UPROPERTY(Transient)
    TObjectPtr<USettingRegistry> OwningRegistry;

    FString Identifier;
    FText   SettingDisplayName;
};
