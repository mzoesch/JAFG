// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "GameSetting.generated.h"

UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UGameSetting : public UObject
{
    GENERATED_BODY()

public:

    explicit UGameSetting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
