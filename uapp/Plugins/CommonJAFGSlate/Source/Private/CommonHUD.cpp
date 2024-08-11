// Copyright 2024 mzoesch. All rights reserved.

#include "CommonHUD.h"
#include "CommonJAFGSlateDeveloperSettings.h"
#include "PopUps/JAFGWarningPopUp.h"
#include "PopUps/JAFGWarningPopUpYesNo.h"
#include "PopUps/LoadingScreen.h"
#include "JAFGMacros.h"

ACommonHUD::ACommonHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header, const TFunction<void()>& OnPopUpClosedDelegate) const
{
    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();
    check( Settings )

    FJAFGWarningPopUpData Data;
    Data.Header                = Header;
    Data.Message               = Message;
    Data.OnPopUpClosedDelegate = OnPopUpClosedDelegate;

    check( Settings->WarningPopUpWidgetClass )

    UJAFGWarningPopUp* WarningPopUp = CreateWidget<UJAFGWarningPopUp>(this->GetWorld(), Settings->WarningPopUpWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const FString& Header) const
{
    this->CreateWarningPopup(Message, Header, [] (void) { });
}

void ACommonHUD::CreateWarningPopup(const FString& Message, const TFunction<void()>& OnPopUpClosedDelegate) const
{
    this->CreateWarningPopup(Message, TEXT("Warning"), OnPopUpClosedDelegate);
}

void ACommonHUD::CreateWarningPopupYesNo(const FString& Message, const FString& Header, const TFunction<void(bool bAccepted)>& OnPopUpClosedDelegate) const
{
    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();
    check( Settings )

    FJAFGWarningPopUpDataYesNo Data;
    Data.Header                     = Header;
    Data.Message                    = Message;
    Data.OnPopUpYesNoClosedDelegate = OnPopUpClosedDelegate;

    check( Settings->WarningPopUpYesNoWidgetClass )

    UJAFGWarningPopUpYesNo* WarningPopUp = CreateWidget<UJAFGWarningPopUpYesNo>(this->GetWorld(), Settings->WarningPopUpYesNoWidgetClass);
    WarningPopUp->PassDataToWidget(Data);

    WarningPopUp->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningPopupYesNo(const FString& Message, const TFunction<void(bool bAccepted)>& OnPopupClosedDelegate) const
{
    this->CreateWarningPopupYesNo(Message, TEXT("Warning"), OnPopupClosedDelegate);
}

void ACommonHUD::CreateLoadingScreen(void)
{
    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();
    jcheck( Settings )

    jcheck( Settings->LoadingScreenWidgetClass )

    this->LoadingScreen = CreateWidget<UJAFGUserWidget>(this->GetWorld(), Settings->LoadingScreenWidgetClass);
    this->LoadingScreen->AddToViewport();

    return;
}

void ACommonHUD::DestroyLoadingScreen(void)
{
    if (this->LoadingScreen)
    {
        this->LoadingScreen->RemoveFromParent();
        this->LoadingScreen = nullptr;
    }

    return;
}
