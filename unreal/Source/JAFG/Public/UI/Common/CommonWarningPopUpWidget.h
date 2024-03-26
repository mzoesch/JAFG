// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/JAFGCommonWidget.h"

#include "CommonWarningPopUpWidget.generated.h"

class ACommonHUD;
class UButton;
class UTextBlock;
class UCommonWarningOptionPopUpWidget;

struct FWarningPopUpWidgetData final : FMyPassData
{
    virtual ~FWarningPopUpWidgetData(void) = default;

    explicit FWarningPopUpWidgetData(const FString& InWarningMessage, const FString& InWarningHeader = FString("Warning"))
        : WarningMessage(InWarningMessage)
        , WarningHeader(InWarningHeader)
    {
    }
    
    FString WarningMessage;
    FString WarningHeader;

private:
    
    /** Optional delegate to be called when the user has chosen an option. Only usefully for UCommonWarningOptionPopUpWidget. */
    TFunction<void(bool bAccepted)> OnOptionChosenDelegate = nullptr;
    friend ACommonHUD;
    /**
     * Kinda sketchy here but this allows as to pass a delegate more explicit than just in this type. So that we do not
     * forget to check the input parameters.
     */
    friend UCommonWarningOptionPopUpWidget;
    
public:

    inline virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("FWarningPopUpWidgetData{WarningHeader: %s, WarningMessage: %s}"), *WarningHeader, *WarningMessage);
    }
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UCommonWarningPopUpWidget : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:
    
    UPROPERTY(BlueprintReadOnly, Category = "WarningPopup", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_CloseWarningPopUp;
    
    UPROPERTY(BlueprintReadOnly, Category = "WarningPopup", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_WarningHeader;

    UPROPERTY(BlueprintReadOnly, Category = "WarningPopup", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_WarningMessage;

protected:

    virtual void NativeConstruct(void) override;

    UFUNCTION()
    FORCEINLINE void OnCloseWarningPopUpClicked() { this->RemoveFromParent(); }
    
public:
    
    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
};

/**
 * Same class as UCommonWarningPopUpWidget but the user should be provided with to options to choose from.
 * Once to abort the action and once to confirm the action.
 */
UCLASS(Abstract, Blueprintable)
class JAFG_API UCommonWarningOptionPopUpWidget : public UCommonWarningPopUpWidget
{
    GENERATED_BODY()

protected:

    UPROPERTY(BlueprintReadOnly, Category = "WarningPopup", meta = (AllowPrivateAccess = "true", BindWidget))
    UButton* B_ConfirmPopUpAction;

public:

    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
    
protected:
    
    virtual void NativeConstruct(void) override;

    UFUNCTION()
    void OnCloseWarningOptionPopUpClicked();
    
    UFUNCTION()
    void OnConfirmPopUpActionClicked();

private:

    TFunction<void(bool bAccepted)> OnOptionChosenDelegate = nullptr;
};
