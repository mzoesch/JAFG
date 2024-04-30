// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"

UGameSettingCollection::UGameSettingCollection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

TArray<UGameSettingCollection*> UGameSettingCollection::GetChildCollections(void) const
{
    TArray<UGameSettingCollection*> CollectionSettings;

    for (UGameSetting* ChildSetting : this->Settings)
    {
        if (UGameSettingCollection* ChildCollection = Cast<UGameSettingCollection>(ChildSetting))
        {
            CollectionSettings.Add(ChildCollection);
        }

        continue;
    }

    return CollectionSettings;
}
