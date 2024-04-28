// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/CommonWarningPopUp.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCommonWarningPopUp::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->B_Close->OnClicked.AddDynamic(this, &UCommonWarningPopUp::OnCloseClicked);

    return;
}

void UCommonWarningPopUp::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FWarningPopUpWidgetData* WarningPopUpWidgetData = static_cast<const FWarningPopUpWidgetData*>(&MyPassedData); WarningPopUpWidgetData == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FWarningPopUpWidgetData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FWarningPopUpWidgetData.")
#endif /* !WITH_EDITOR */
    }
    else
    {
        this->TB_Header->SetText(FText::FromString(WarningPopUpWidgetData->Header));
        this->TB_Body->SetText(FText::FromString(WarningPopUpWidgetData->Body));
    }

    this->OnDeferredConstruct();

    return;
}

void UCommonWarningPopUpYesNo::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->B_Close->OnClicked.Clear();
    this->B_Close->OnClicked.AddDynamic(this, &UCommonWarningPopUpYesNo::OnActionNoClicked);
    this->B_Confirm->OnClicked.AddDynamic(this, &UCommonWarningPopUpYesNo::OnActionYesClicked);

    return;
}

void UCommonWarningPopUpYesNo::OnActionNoClicked(void)
{
    this->RemoveFromParent();

    if (this->OnWarningActionChosenEvent.IsBound())
    {
        this->OnWarningActionChosenEvent.Execute(false);
        return;
    }

    LOG_ERROR(LogCommonSlate, "Not bound.")

    return;
}

void UCommonWarningPopUpYesNo::OnActionYesClicked(void)
{
    this->RemoveFromParent();

    if (this->OnWarningActionChosenEvent.IsBound())
    {
        this->OnWarningActionChosenEvent.Execute(true);
        return;
    }

    LOG_ERROR(LogCommonSlate, "Not bound.")

    return;
}
