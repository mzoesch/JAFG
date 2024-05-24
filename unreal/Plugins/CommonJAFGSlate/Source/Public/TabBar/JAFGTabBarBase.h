// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "JAFGTabBarBase.generated.h"

class UJAFGTabBarButton;
class UJAFGTabBar;
class UJAFGTabBarBase;

/** Identifier marks the new active panel. */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTabPressedSignature, const FString& /* Identifier */)

struct COMMONJAFGSLATE_API FButtonEntryDescriptor : public FWidgetPassData
{
    FString Identifier = L"";
    TObjectPtr<UJAFGTabBarBase> Owner;
};

/**
 * Base class to allow nesting of tab bars.
 * @see JAFGTabBar.h
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGTabBarBase : public UJAFGUserWidget
{
    GENERATED_BODY()

    friend UJAFGTabBar;
    friend UJAFGTabBarButton;

protected:

    /** For tab bar panels. */
    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnMadeVisible( /* void */ );
    /** For tab bar panels. */
    virtual void OnNativeMadeVisible(void) { }

    /** For tab bar panels. */
    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnMadeCollapsed( /* void */ );
    /** For tab bar panels. */
    virtual void OnNativeMadeCollapsed(void) { }
};
