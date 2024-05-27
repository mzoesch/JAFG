// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"

#include "GameSettingCollections.generated.h"

/** A collection of settings. Is nestable. */
UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UGameSettingCollection : public UGameSetting
{
    GENERATED_BODY()

public:

    explicit UGameSettingCollection(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UGameSetting implementation
    virtual auto GetChildSettings(void) const -> TArray<UGameSetting*> override { return OwnedSettings; }
    // ~UGameSetting implementation

    auto AddSetting(UGameSetting* InSetting) -> void;
    auto GetSettingByIdentifier(const FString& InIdentifier) const -> UGameSetting*;

protected:

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> OwnedSettings;
};

/**
 * Allows for lazy body initialization of a collection.
 * A lazy collection must be *manually* initialized if it is accessed, and you are not sure about the initialization
 * state of the concrete collection. It is always safe to call the Lazy Initialize method multiple times.
 * The UGameSetting#Identifier cannot be lazily initialized.
 */
UCLASS(NotBlueprintType)
class COMMONSETTINGS_API ULazyGameSettingCollection : public UGameSettingCollection
{
    GENERATED_BODY()

public:

    /** Call this method *manually* when a collection should be initialized. */
    virtual void LazyInitialize(UCustomSettingsLocalPlayer* InOwningPlayer);

    void SetLazyInitFunction(const TFunction<void(UCustomSettingsLocalPlayer* InOwningPlayer)>& InLazyInitFunction);

private:

    bool bLazyInitialized = false;

    /**
     * The lazy initialization function is executed, as it is to be expected in a normal collection, before the
     * GameSetting#Initialize method is called.
     * The lazy initialization function is meant to define the body of the concrete collection, meaning all
     * subcollections and settings should be added there.
     * This is useful if the collection depends on settings that are added at runtime during local player
     * initialization.
     */
    TFunction<void(UCustomSettingsLocalPlayer* InOwningPlayer)> LazyInitFunction = nullptr;
};
