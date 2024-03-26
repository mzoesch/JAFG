// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/CommonWarningPopUpWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCommonWarningPopUpWidget::NativeConstruct()
{
    Super::NativeConstruct();
    this->B_CloseWarningPopUp->OnClicked.AddDynamic(this, &UCommonWarningPopUpWidget::OnCloseWarningPopUpClicked);
    return;
}

void UCommonWarningPopUpWidget::PassDataToWidget(const FMyPassData& MyPassedData)
{
    if (const FWarningPopUpWidgetData* WarningPopUpWidgetData = static_cast<const FWarningPopUpWidgetData*>(&MyPassedData); WarningPopUpWidgetData == nullptr)
    {
#if WITH_EDITOR
        UE_LOG(LogTemp, Error, TEXT("UCommonWarningPopupWidget::PassDataToWidget: MyPassedData is not of type FWarningPopUpWidgetData."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("UCommonWarningPopupWidget::PassDataToWidget: MyPassedData is not of type FWarningPopUpWidgetData."))
#endif /* WITH_EDITOR */
    }
    else
    {
        this->TB_WarningHeader->SetText(FText::FromString(WarningPopUpWidgetData->WarningHeader));
        this->TB_WarningMessage->SetText(FText::FromString(WarningPopUpWidgetData->WarningMessage));
    }

    this->OnDeferredConstruct();

    return;
}

void UCommonWarningOptionPopUpWidget::PassDataToWidget(const FMyPassData& MyPassedData)
{
    Super::PassDataToWidget(MyPassedData);

    if (const FWarningPopUpWidgetData* WarningPopUpWidgetData = static_cast<const FWarningPopUpWidgetData*>(&MyPassedData); WarningPopUpWidgetData == nullptr)
    {
#if WITH_EDITOR
        UE_LOG(LogTemp, Error, TEXT("{UCommonWarningOptionPopUpWidget::PassDataToWidget: MyPassedData is not of type FWarningPopUpWidgetData."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("UCommonWarningOptionPopUpWidget::PassDataToWidget: MyPassedData is not of type FWarningPopUpWidgetData."))
#endif /* WITH_EDITOR */
    }
    else
    {
        this->OnOptionChosenDelegate = WarningPopUpWidgetData->OnOptionChosenDelegate;
    }

    /* this->OnDeferredConstruct(); */
    
    return;
}

void UCommonWarningOptionPopUpWidget::NativeConstruct()
{
    Super::NativeConstruct();

    /** Just to be safe and clear up the parent delegates. */
    this->B_CloseWarningPopUp->OnClicked.Clear();
    
    this->B_CloseWarningPopUp->OnClicked.AddDynamic(this, &UCommonWarningOptionPopUpWidget::OnCloseWarningOptionPopUpClicked);
    this->B_ConfirmPopUpAction->OnClicked.AddDynamic(this, &UCommonWarningOptionPopUpWidget::OnConfirmPopUpActionClicked);
    
    return;
}

void UCommonWarningOptionPopUpWidget::OnCloseWarningOptionPopUpClicked()
{
    if (this->OnOptionChosenDelegate == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UCommonWarningOptionPopupWidget::OnConfirmPopUpActionClicked: OnOptionChosenDelegate is not bound."))
        return;
    }

    this->OnOptionChosenDelegate(false);

    Super::OnCloseWarningPopUpClicked();

    return;
}

void UCommonWarningOptionPopUpWidget::OnConfirmPopUpActionClicked()
{
    if (this->OnOptionChosenDelegate == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UCommonWarningOptionPopupWidget::OnConfirmPopUpActionClicked: OnOptionChosenDelegate is not bound."))
        return;
    }

    this->OnOptionChosenDelegate(true);
    
    Super::OnCloseWarningPopUpClicked();

    return;
}
