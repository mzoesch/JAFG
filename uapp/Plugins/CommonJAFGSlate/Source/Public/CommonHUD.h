// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "CommonHUD.generated.h"

class UJAFGUserWidget;

UCLASS(NotBlueprintable)
class COMMONJAFGSLATE_API ACommonHUD : public AHUD
{
    GENERATED_BODY()

public:

    explicit ACommonHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    auto CreateWarningPopup(const FString& Message, const FString& Header = TEXT("Warning")) const -> void;
    auto CreateWarningPopup(const FString& Message, const FString& Header, const TFunction<void(bool bAccepted)>& OnPopUpClosedDelegate) const -> void;
    auto CreateWarningPopup(const FString& Message, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate) const -> void;

    auto CreateLoadingScreen(void) -> void;
    auto DestroyLoadingScreen(void) -> void;

private:

    UPROPERTY()
    TObjectPtr<UJAFGUserWidget> LoadingScreen;
};
