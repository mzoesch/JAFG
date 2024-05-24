// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "JAFGTabBarButton.generated.h"

class UJAFGFocusableUserWidget;
class UTextBlock;
class UButton;
class UJAFGTabBarBase;

/** @see JAFGTabBar.h */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGTabBarButton : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    /**
     * Call this method manually from kismet if this tab was pressed or focused in any way, and the
     * UJAFGTabBarButton#B_MasterButton is not bound.
     */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    virtual void OnThisTabPressed( /* void */ ) { this->NativeOnThisTabPressed(); }
    /** Automatically called it the UJAFGTabBarButton#B_MasterButton is bound. */
    virtual void NativeOnThisTabPressed(void);

    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnFocusTab( /* void */ );
    void NativeOnFocusTab(void) const;
    UFUNCTION(BlueprintImplementableEvent, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    void OnUnfocusTab( /* void */ );
    void NativeOnUnfocusTab(void) const;

    /** Will be called if any tab in the collection was pressed. */
    virtual void OnTabPressed(const FString& Identifier);

    /** Generalized to allow nesting. */
    UPROPERTY()
    TObjectPtr<UJAFGTabBarBase> OwningTabBar = nullptr;
    template<typename T> T* GetOwningTabBar(void) const { return Cast<T>(this->OwningTabBar); }
    template<typename T> T* GetCheckedOwningTabBar(void) const { return CastChecked<T>(this->OwningTabBar); }
    FString TabIdentifier = L"";

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    TObjectPtr<UJAFGFocusableUserWidget> W_TargetFocusableWidget;

    /** Is unnecessary if UJAFGTabBarButton#W_TargetFocusableWidget is bound. */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    TObjectPtr<UTextBlock> TB_ButtonText;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget", meta = (AllowPrivateAccess = "true"))
    FString GetTextForButton( /* void */ ) const { return this->TabIdentifier; }

    virtual void InitializeTab(void);
};
