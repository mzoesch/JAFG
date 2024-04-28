// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWidget.h"

#include "CommonWarningPopUp.generated.h"

DECLARE_DELEGATE_OneParam(FOnWarningActionChosenEventSignature, bool /* bAccepted */)

JAFG_VOID

class UTextBlock;
class UButton;

struct JAFGCORE_API FWarningPopUpWidgetData final : FMyPassedData
{
    virtual ~FWarningPopUpWidgetData(void) = default;

    explicit FWarningPopUpWidgetData(const FString& InBody, const FString& InHeader = FString("Warning"))
        : Body(InBody)
        , Header(InHeader)
    {
        return;
    }

    FString Body;
    FString Header;

    FORCEINLINE virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("FWarningPopUpWidgetData{Header: %s, Body: %s}"), *Header, *Body);
    }
};

UCLASS(Abstract, Blueprintable)
class JAFGCORE_API UCommonWarningPopUp : public UJAFGWidget
{
    GENERATED_BODY()

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UButton> B_Close;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UTextBlock> TB_Header;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UTextBlock> TB_Body;

    virtual void NativeConstruct(void) override;

    UFUNCTION()
    FORCEINLINE void OnCloseClicked() { this->RemoveFromParent(); }

public:

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;
};


UCLASS(Abstract, Blueprintable)
class UCommonWarningPopUpYesNo : public UCommonWarningPopUp
{
    GENERATED_BODY()

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UButton> B_Confirm;

    virtual void NativeConstruct(void) override;

    UFUNCTION()
    void OnActionNoClicked( /* void */ );

    UFUNCTION()
    void OnActionYesClicked( /* void */ );

public:

    FOnWarningActionChosenEventSignature OnWarningActionChosenEvent;
};
