// Copyright 2024 mzoesch. All rights reserved.

#include "CommonHUD.h"

#include "JAFGSlateSettings.h"
#include "WarningPopUp.h"
#include "WarningPopUpYesNo.h"

ACommonHUD::ACommonHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header)
{
    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();
    check( SlateSettings )

    FWarningPopUpData Data;
    Data.Header  = Header;
    Data.Message = Message;

    check( SlateSettings->WarningPopUpWidgetClass )

    UWarningPopUp* WarningPopUp = CreateWidget<UWarningPopUp>(this->GetWorld(), SlateSettings->WarningPopUpWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate)
{
    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();
    check( SlateSettings )

    FWarningPopUpYesNoData Data;
    Data.Header                = Header;
    Data.Message               = Message;
    Data.OnPopupClosedDelegate = OnPopupClosedDelegate;

    check( SlateSettings->WarningPopUpYesNoWidgetClass )

    UWarningPopUpYesNo* WarningPopUp = CreateWidget<UWarningPopUpYesNo>(this->GetWorld(), SlateSettings->WarningPopUpYesNoWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}
