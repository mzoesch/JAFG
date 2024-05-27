// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingCollections.h"

#include "JAFGLogDefs.h"
#include "SettingsData/GameSettingRegistry.h"

UGameSettingCollection::UGameSettingCollection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingCollection::AddSetting(UGameSetting* InSetting)
{
    if (InSetting == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Cannot add a null setting to a collection.")
        return;
    }

    if (this->OwnedSettings.Contains(InSetting))
    {
        LOG_FATAL(LogGameSettings, "Setting [] is already owned by this collection.", *InSetting->GetIdentifier())
        return;
    }

    if (this->GetSettingByIdentifier(InSetting->GetIdentifier()))
    {
        LOG_FATAL(LogGameSettings, "Setting [] is already owned by this collection.", *InSetting->GetIdentifier())
        return;
    }

    this->OwnedSettings.Add(InSetting);

    return;
}

UGameSetting* UGameSettingCollection::GetSettingByIdentifier(const FString& InIdentifier) const
{
    for (UGameSetting* Setting : this->OwnedSettings)
    {
        check( Setting )

        if (Setting->GetIdentifier() == InIdentifier)
        {
            return Setting;
        }

        if (const UGameSettingCollection* SubCollection = Cast<UGameSettingCollection>(Setting))
        {
            if (
                UGameSetting* FoundSetting = SubCollection->GetSettingByIdentifier(InIdentifier);
                FoundSetting != nullptr
            )
            {
                return FoundSetting;
            }
        }
    }

    return nullptr;
}

void ULazyGameSettingCollection::LazyInitialize(UCustomSettingsLocalPlayer* InOwningPlayer)
{
    if (this->bLazyInitialized)
    {
        LOG_WARNING(LogGameSettings, "Collection [%s] is already initialized.", *this->Identifier)
        return;
    }

    this->bLazyInitialized = true;

    if (this->LazyInitFunction)
    {
        this->LazyInitFunction.CheckCallable();
        this->LazyInitFunction(InOwningPlayer);
    }
    else
    {
        LOG_ERROR(
            LogGameSettings,
            "No lazy init function set for lazy collection [%s]. Should properly be reparented.",
            *this->Identifier
        )
    }

    this->Initialize(InOwningPlayer);

    if (this->OwningRegistry == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Owning registry is invalid for lazy collection [%s].", *this->Identifier)
        return;
    }

    /*
     * If this is a top level lazy setting, it should already have been registered as one. As the identifier cannot
     * be set lazily, and a minimal number of settings is required also for lazy settings.
     */
    this->OwningRegistry->RegisterInnerSetting(this);

    return;
}

void ULazyGameSettingCollection::SetLazyInitFunction(const TFunction<void(UCustomSettingsLocalPlayer* InOwningPlayer)>& InLazyInitFunction)
{
    if (this->Identifier.IsEmpty())
    {
        LOG_FATAL(LogGameSettings, "Identifier is empty but tried to lazy initialize.")
        return;
    }

    this->LazyInitFunction = InLazyInitFunction;

    return;
}
