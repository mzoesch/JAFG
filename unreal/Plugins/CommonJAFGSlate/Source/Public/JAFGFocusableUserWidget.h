// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "JAFGFocusableUserWidget.generated.h"

class UButton;
class UTextBlock;

typedef TFunction<void(int32 WidgetIdentifier)> FFocusableWidgetDelegate;

struct FPassedFocusableWidgetData : public FWidgetPassData
{
    int32                    WidgetIdentifier        = 0;
    FFocusableWidgetDelegate OnWidgetPressedDelegate = nullptr;
};

/** A widget that can be focused with the default logic for focusing widgets in JAFG. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGFocusableUserWidget : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGFocusableUserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

    void SetTextIfBound(const FString& InText) const;

    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetFocusWithBool(const bool bFocus);

    /**
     * We do not focus automatically here, but let the owing collection handle the logic for that and then
     * wait for callback here to set the focus.
     */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetFocus( /* void */ );

    /**
     * We do not unfocus automatically here, but let the owing collection handle the logic for that and then
     * wait for callback here to set the focus.
     */
    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetUnfocus( /* void */ );

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_Master = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget, OptionalWidget))
    UTextBlock* TB_Master = nullptr;

    UPROPERTY(BlueprintReadOnly)
    FButtonStyle ButtonStyleUnfocused;

    UPROPERTY(BlueprintReadOnly)
    FButtonStyle ButtonStyleFocused;

private:

    UFUNCTION()
    void OnClickedDynamic( /* void */ );

    int32                    WidgetIdentifier        = 0;
    FFocusableWidgetDelegate OnWidgetPressedDelegate = nullptr;
};
