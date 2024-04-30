// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGSettingScreen.h"
#include "MyCore.h"
#include "FrontEnd/JAFGWidget.h"

#include "SettingScreenSideTab.generated.h"

JAFG_VOID

struct FSettingTabDescriptor;

struct COMMONOSD_API FPassedSettingTabDescriptor final : FMyPassedData
{
    virtual ~FPassedSettingTabDescriptor(void) = default;

    FSettingTabDescriptor Descriptor;

    virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("Descriptor: %s"), *this->Descriptor.DisplayName);
    }
};

UCLASS(Abstract, Blueprintable)
class COMMONOSD_API USettingScreenSideTab : public UJAFGWidget
{
    GENERATED_BODY()

    FSettingTabDescriptor TabDescriptor;

public:

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;

    UFUNCTION(BlueprintPure, Category = "JAFG|Settings")
    FString GetSettingDisplayName( /* void */ ) const { return this->TabDescriptor.DisplayName; }
};

