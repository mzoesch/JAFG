// Copyright 2024 mzoesch. All rights reserved.

#include "PopUps/JAFGWarningPopUpYesNo.h"

#include "Components/Button.h"

UJAFGWarningPopUpYesNo::UJAFGWarningPopUpYesNo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGWarningPopUpYesNo::NativeConstruct(void)
{
    Super::NativeConstruct();

    if (this->B_CloseButton)
    {
        this->B_CloseButton->OnClicked.Clear();
        this->B_CloseButton->OnClicked.AddDynamic(this, &UJAFGWarningPopUpYesNo::OnNo);
    }

    if (this->B_ConfirmButton)
    {
        this->B_ConfirmButton->OnClicked.AddDynamic(this, &UJAFGWarningPopUpYesNo::OnYes);
    }

    return;
}

void UJAFGWarningPopUpYesNo::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FJAFGWarningPopUpDataYesNo)
    {
        this->OnPopupClosedDelegate = Data->OnPopUpClosedDelegate;
    }

    return;
}

void UJAFGWarningPopUpYesNo::OnYesNoClose(const bool bAccepted)
{
    if (this->OnPopupClosedDelegate)
    {
        this->OnPopupClosedDelegate(bAccepted);
    }
    else
    {
        LOG_ERROR(LogCommonSlate, "On Popup Closed Delegate is not bound.")
    }

    Super::OnClose();

    return;
}

void UJAFGWarningPopUpYesNo::OnYes(void)
{
    this->OnYesNoClose(true);
}

void UJAFGWarningPopUpYesNo::OnNo()
{
    this->OnYesNoClose(false);
}
