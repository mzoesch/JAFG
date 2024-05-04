// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "CommonHUD.generated.h"

UCLASS(NotBlueprintable)
class JAFGSLATECORE_API ACommonHUD : public AHUD
{
    GENERATED_BODY()

public:

    explicit ACommonHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

                auto CreateWarningPopup(const FString& Message, const FString& Header = TEXT("Warning")) -> void;
                auto CreateWarningPopup(const FString& Message, const FString& Header, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate) -> void;
    FORCEINLINE void CreateWarningPopup(const FString& Message, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate)
    {
        this->CreateWarningPopup(Message, TEXT("Warning"), OnPopupClosedDelegate);
    }
};
