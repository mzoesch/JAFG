// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTabBarBase.h"

#include "JAFGTabBar.generated.h"

class UWidgetSwitcher;
class UJAFGTabBarButton;

struct COMMONJAFGSLATE_API FTabBarTabDescriptor final
{
    FTabBarTabDescriptor(void) { }
    ~FTabBarTabDescriptor(void) = default;

    FString Identifier  = L"";
    FString DisplayName = L"";

    TSubclassOf<UJAFGTabBarButton> ButtonWidgetClass = nullptr;
    TSubclassOf<UJAFGTabBarBase>   PanelWidgetClass  = nullptr;

    FMargin Padding  = FMargin(0.0f);
};

/**
 * A tab bar with the selection and focusable logic that is commonly used in JAFG.
 * The tab bar consists of three main parts:
 *   - The Tab Bar Button: The button that represents the tab in a given collection.
 *   - The Tab Bar Panel:  The panel that represents the content of the tab.
 *   - The Tab Bar:        The collection of the buttons and panels.
 *
 * Tab bars can be nested within each other with the superclass UJAFGTabBarBase.
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGTabBar : public UJAFGTabBarBase
{
    GENERATED_BODY()

public:

    explicit UJAFGTabBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    FOnTabPressedSignature OnTabPressedEvent;

    /** Will focus the No Active Tab. */
    void UnfocusAllTabs(void);

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> P_EntryButtonContainer;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UWidgetSwitcher> WS_PanelContainer;

    virtual auto RegisterAllTabs(void) -> void { }
            auto RegisterConcreteTab(const FTabBarTabDescriptor& TabDescriptor) -> void;
    /** Will fill subclass references with the project defaults. */
    static  auto GetDefaultTabDescriptor(void) -> FTabBarTabDescriptor;

private:

    /** Called manually by the button entries themselves when they receive their pressed event. */
    void OnTabPressed(const FString& Identifier);
    friend UJAFGTabBarButton;

    inline static const FString NoActiveTabIdentifier = L"";
                        FString ActiveTabIdentifier   = UJAFGTabBar::NoActiveTabIdentifier;

    /** Order is based on the children of the UCommonBarWidget#WS_PanelContainer. */
    TArray<FString> RegisteredTabIdentifiersInOrder;
    /** @return The index in the Widget Switcher of the specific identifier. -1 if it was not found. */
    auto FindIndexOfIdentifier(const FString& Identifier) const -> int32;
};
