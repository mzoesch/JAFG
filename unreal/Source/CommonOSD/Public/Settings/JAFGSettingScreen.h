// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "FrontEnd/JAFGWidget.h"

#include "JAFGSettingScreen.generated.h"

class UVerticalBox;
class USettingScreenSideTab;
JAFG_VOID

class USettingRegistry;
class UGameSettingCollection;

USTRUCT(BlueprintType)
struct FSettingTabDescriptor
{
    GENERATED_BODY()

    FSettingTabDescriptor(void)
    {
    }
    ~FSettingTabDescriptor(void) = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;
};

UCLASS(Abstract, Blueprintable)
class COMMONOSD_API UJAFGSettingScreen : public UJAFGWidget
{
    GENERATED_BODY()

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UVerticalBox> VB_SideTabs;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<USettingScreenSideTab> SideTabClass;

    UFUNCTION(BlueprintCallable, Category = "JAFG|Menu", meta = (AllowPrivateAccess = "true"))
    void RegisterTab(const FSettingTabDescriptor& Descriptor);

    UFUNCTION(BlueprintPure, Category = "JAFG|Settings", meta = (AllowPrivateAccess = "true"))
    UGameSettingCollection* GetSettingCollection(const FString& Identifier) const;

private:

    USettingRegistry* GetRegistry(void) const;
};
