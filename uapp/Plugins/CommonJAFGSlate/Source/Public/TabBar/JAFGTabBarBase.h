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
    FString Identifier  = L"";
    FString DisplayName = L"";
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

public:

    explicit UJAFGTabBarBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    // ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
    void OnMadeVisible( /* void */ );
    virtual void OnNativeMadeVisible(void) { }

    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    // ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
    void OnMadeCollapsed( /* void */ );
    virtual void OnNativeMadeCollapsed(void) { }

    virtual bool AllowClose(void) const { return true; }
    virtual void TryToClose(const TFunction<void(void)>& CallbackIfLateAllow) { }
};
