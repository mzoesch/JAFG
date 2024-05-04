// Copyright 2024 mzoesch. All rights reserved.

#include "WarningPopUpYesNo.h"

#include "Components/Button.h"

UWarningPopUpYesNo::UWarningPopUpYesNo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->B_ConfirmButton = nullptr;
    return;
}

void UWarningPopUpYesNo::NativeConstruct(void)
{
    Super::NativeConstruct();

    if (this->B_CloseButton)
    {
        this->B_CloseButton->OnClicked.Clear();
        this->B_CloseButton->OnClicked.AddDynamic(this, &UWarningPopUpYesNo::OnNo);
    }

    if (this->B_ConfirmButton)
    {
        this->B_ConfirmButton->OnClicked.AddDynamic(this, &UWarningPopUpYesNo::OnYes);
    }

    return;
}

void UWarningPopUpYesNo::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    Super::PassDataToWidget(MyPassedData);

    if (const FWarningPopUpYesNoData* Data = static_cast<const FWarningPopUpYesNoData*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FWarningPopUpYesNoData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FWarningPopUpYesNoData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->OnPopupClosedDelegate = Data->OnPopupClosedDelegate;
    }

    /*
     * Deferred construct already called in super PassDataToWidget
     */

    return;
}

void UWarningPopUpYesNo::OnYesNoClose(const bool bAccepted)
{
    if (this->OnPopupClosedDelegate)
    {
        this->OnPopupClosedDelegate(bAccepted);
    }
    else
    {
        LOG_ERROR(LogCommonSlate, "OnPopupClosedDelegate is not bound.")
    }

    Super::OnClose();

    return;
}

void UWarningPopUpYesNo::OnYes(void)
{
    this->OnYesNoClose(true);
}

void UWarningPopUpYesNo::OnNo(void)
{
    this->OnYesNoClose(false);
}
