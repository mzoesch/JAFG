// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingCollection.h"

#include "JAFGLogDefs.h"

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
