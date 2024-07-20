// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "GameSettingListEntry.generated.h"

class UJAFGTextBlock;
class USettingsTabBarPanel;

struct COMMONSETTINGS_API FGameSettingListEntryPassData : public FWidgetPassData
{
    FText SettingName;
    USettingsTabBarPanel* OwningPanel;
};

UCLASS(Abstract, Transient, NotBlueprintable)
class COMMONSETTINGS_API UGameSettingListEntry : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UGameSettingListEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    virtual void OnRestoreSettingsToInitial(void) { }

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGTextBlock> Text_SettingName;

    UPROPERTY()
    TObjectPtr<USettingsTabBarPanel> OwningPanel;
};
