// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGWidget.h"

#include "JAFGFocusableWidget.generated.h"

class UButton;

struct FPassedFocusableWidgetData : public FMyPassedData
{
    virtual ~FPassedFocusableWidgetData(void) = default;

    int32 WidgetIdentifier                 = 0;
    TFunction<void(int32 WidgetIdentifier)> OnWidgetPressedDelegate = nullptr;
};

UCLASS(Abstract, blueprintable)
class JAFGSLATECORE_API UJAFGFocusableWidget : public UJAFGWidget
{
    GENERATED_BODY()

public:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_Master = nullptr;

    UPROPERTY(BlueprintReadOnly)
    FButtonStyle ButtonStyleUnfocused;

    UPROPERTY(BlueprintReadOnly)
    FButtonStyle ButtonStyleFocused;

public:

    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetFocusWithBool(const bool bFocus);

    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetFocus( /* void */ );

    UFUNCTION(BlueprintCallable, Category = "JAFG|Focusable", meta = (AllowPrivateAccess = "true"))
    virtual void SetWidgetUnfocus( /* void */ );

private:

    UFUNCTION()
    void OnPressedDynamic( /* void */ );

    int32                                   WidgetIdentifier        = 0;
    TFunction<void(int32 WidgetIdentifier)> OnWidgetPressedDelegate = nullptr;
};
