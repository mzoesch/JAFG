// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTabBarButton.h"

#include "JAFGTabBarButtonPopUp.generated.h"

/** Modifies the on click behavior with displaying a pop-up. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGTabBarButtonPopUp : public UJAFGTabBarButton
{
    GENERATED_BODY()

public:

    explicit UJAFGTabBarButtonPopUp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    /** Override to allow for more pop-ups than simple ones and yes no. */
    virtual void NativeOnThisTabPressed(void) override;

    /** Here add custom behavior in derived class. Only called if yes no. */
    virtual void OnPopUpClosed(const bool bAccepted) { }

    FString Message  = L"";
    FString Header   = L"";
    bool    bIsYesNo = false;
};
