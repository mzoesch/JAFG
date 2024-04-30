// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameSetting.h"

#include "GameSettingCollection.generated.h"

JAFG_VOID

UCLASS()
class JAFGCORE_API UGameSettingCollection : public UGameSetting
{
    GENERATED_BODY()

public:

    explicit UGameSettingCollection(const FObjectInitializer& ObjectInitializer = FObjectInitializer());

    FORCEINLINE virtual auto GetChildSettings(void) -> TArray<UGameSetting*>  override { return this->Settings; }
                        auto GetChildCollections(void) const -> TArray<UGameSettingCollection*>;

protected:

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> Settings;
};
