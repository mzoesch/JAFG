// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"

#include "GameSettingCollection.generated.h"

UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UGameSettingCollection : public UGameSetting
{
    GENERATED_BODY()

public:

    explicit UGameSettingCollection(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
