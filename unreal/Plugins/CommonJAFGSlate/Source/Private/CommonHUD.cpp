// Copyright 2024 mzoesch. All rights reserved.

#include "CommonHUD.h"

#include "CommonJAFGSlateDeveloperSettings.h"
#include "PopUps/JAFGWarningPopUp.h"
#include "PopUps/JAFGWarningPopUpYesNo.h"

ACommonHUD::ACommonHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header) const
{
    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();
    check( Settings )

    FJAFGWarningPopUpData Data;
    Data.Header  = Header;
    Data.Message = Message;

    check( Settings->WarningPopUpWidgetClass )

    UJAFGWarningPopUp* WarningPopUp = CreateWidget<UJAFGWarningPopUp>(this->GetWorld(), Settings->WarningPopUpWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header, const TFunction<void(bool bAccepted)>& OnPopUpClosedDelegate) const
{
    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();
    check( Settings )

    FJAFGWarningPopUpDataYesNo Data;
    Data.Header                = Header;
    Data.Message               = Message;
    Data.OnPopUpClosedDelegate = OnPopUpClosedDelegate;

    check( Settings->WarningPopUpYesNoWidgetClass )

    UJAFGWarningPopUpYesNo* WarningPopUp = CreateWidget<UJAFGWarningPopUpYesNo>(this->GetWorld(), Settings->WarningPopUpYesNoWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate) const
{
    this->CreateWarningPopup(Message, TEXT("Warning"), OnPopupClosedDelegate);
}
