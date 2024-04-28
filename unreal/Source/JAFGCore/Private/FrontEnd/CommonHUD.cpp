// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/CommonHUD.h"

#include "FrontEnd/CommonWarningPopUp.h"

void ACommonHUD::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->WarningPopUpWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Warning pop-up widget class is not valid.")
        return;
    }

    if (this->WarningPopUpYesNoWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Warning pop-up yes/no widget class is not valid.")
        return;
    }

    return;
}

void ACommonHUD::CreateWarningPopUp(const FString& Body, const FString& Header) const
{
    this->CreateWarningPopUp(FWarningPopUpWidgetData(Body, Header));
}

void ACommonHUD::CreateWarningPopUp(const FWarningPopUpWidgetData& Data) const
{
    if (Data.Header.IsEmpty() || Data.Body.IsEmpty())
    {
        LOG_FATAL(LogCommonSlate, "Data is not valid.")
        return;
    }

    LOG_VERBOSE(LogCommonSlate, "Creating warning pop-up.")

    UCommonWarningPopUp* Widget = CreateWidget<UCommonWarningPopUp>(this->GetWorld(), this->WarningPopUpWidgetClass);
    Widget->PassDataToWidget(Data);
    Widget->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopUp(const FString& Body, const FString& Header, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const
{
    this->CreateWarningPopUp(FWarningPopUpWidgetData(Body, Header), OnOptionChosenDelegate);
}

void ACommonHUD::CreateWarningPopUp(const FWarningPopUpWidgetData& Data, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const
{
    if (Data.Header.IsEmpty() || Data.Body.IsEmpty())
    {
        LOG_FATAL(LogCommonSlate, "Data is not valid.")
        return;
    }

    LOG_VERBOSE(LogCommonSlate, "Creating warning pop-up.")

    UCommonWarningPopUpYesNo* Widget = CreateWidget<UCommonWarningPopUpYesNo>(this->GetWorld(), this->WarningPopUpYesNoWidgetClass);
    Widget->OnWarningActionChosenEvent.BindLambda(OnOptionChosenDelegate);
    Widget->PassDataToWidget(Data);
    Widget->AddToViewport();

    return;
}
