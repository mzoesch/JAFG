// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "CommonHUD.generated.h"

class UCommonWarningPopUpWidget;
class UCommonWarningOptionPopUpWidget;
struct FWarningPopUpWidgetData;

UCLASS(Abstract, Blueprintable)
class JAFG_API ACommonHUD : public AHUD
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCommonWarningPopUpWidget> WarningPopUpWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCommonWarningOptionPopUpWidget> WarningPopUpOptionWidgetClass;
    
public:
    
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    void CreateWarningPopUp(const FString& WarningMessage, const FString& WarningHeader = FString("Warning")) const;
    void CreateWarningPopUp(const FWarningPopUpWidgetData& Data) const;

    void CreateWarningOptionPopUp(FWarningPopUpWidgetData& Data, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const;
};
