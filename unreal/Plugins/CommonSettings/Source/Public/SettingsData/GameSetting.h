// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "GameSetting.generated.h"

class UGameSettingRegistry;
class UCustomSettingsLocalPlayer;

UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UGameSetting : public UObject
{
    GENERATED_BODY()

public:

    explicit UGameSetting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void Initialize(UCustomSettingsLocalPlayer* InOwningPlayer);

    FORCEINLINE virtual auto GetChildSettings(void) const -> TArray<UGameSetting*> { return TArray<UGameSetting*>(); }

    FORCEINLINE auto SetOwningRegistry(UGameSettingRegistry* InRegistry) -> void { this->OwningRegistry = InRegistry; }
    FORCEINLINE auto GetOwningRegistry(void) const -> UGameSettingRegistry* { return this->OwningRegistry.Get(); }

    FORCEINLINE auto GetIdentifier(void) const -> FString { return this->Identifier; }
    FORCEINLINE auto SetIdentifier(const FString& InIdentifier) -> void { this->Identifier = InIdentifier; }
    FORCEINLINE auto GetDisplayName(void) const -> FText { return this->DisplayName; }
    FORCEINLINE auto SetDisplayName(const FText& InDisplayName) -> void { this->DisplayName = InDisplayName; }
    FORCEINLINE auto SetDisplayName(const FString& InDisplayName) -> void { this->SetDisplayName(FText::FromString(InDisplayName)); }

protected:

    virtual void OnInitialized(void) { }

    UPROPERTY(Transient)
    TObjectPtr<UCustomSettingsLocalPlayer> OwningPlayer;

    UPROPERTY(Transient)
    TObjectPtr<UGameSettingRegistry> OwningRegistry;

    /** Must always unique inside a registry. */
    FString Identifier  = L"";
    FText   DisplayName = FText::GetEmpty();
};
