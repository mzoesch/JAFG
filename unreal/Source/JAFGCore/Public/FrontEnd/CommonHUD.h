// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/HUD.h"

#include "CommonHUD.generated.h"

JAFG_VOID

class UCommonWarningPopUpYesNo;
class UCommonWarningPopUp;
struct FWarningPopUpWidgetData;

UCLASS(Abstract, Blueprintable)
class JAFGCORE_API ACommonHUD : public AHUD
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "OSD", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCommonWarningPopUp> WarningPopUpWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "OSD", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCommonWarningPopUpYesNo> WarningPopUpYesNoWidgetClass;

public:

    virtual void BeginPlay(void) override;

    UFUNCTION(BlueprintCallable, Category = "OSD", meta = (AllowPrivateAccess = "true"))
    void CreateWarningPopUp(const FString& Body, const FString& Header = FString("Warning")) const;
    void CreateWarningPopUp(const FWarningPopUpWidgetData& Data) const;
    void CreateWarningPopUp(const FString& Body, const FString& Header, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const;
    void CreateWarningPopUp(const FWarningPopUpWidgetData& Data, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const;
};
