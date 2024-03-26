// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/CommonHUD.h"

#include "UI/Common/CommonWarningPopUpWidget.h"

void ACommonHUD::BeginPlay()
{
    Super::BeginPlay();

    check( this->WarningPopUpWidgetClass )
    check( this->WarningPopUpOptionWidgetClass )

    return;
}

void ACommonHUD::CreateWarningPopUp(const FString& WarningMessage, const FString& WarningHeader) const
{
    this->CreateWarningPopUp(FWarningPopUpWidgetData(WarningHeader, WarningMessage));
}

void ACommonHUD::CreateWarningPopUp(const FWarningPopUpWidgetData& Data) const
{
    if (Data.WarningHeader.IsEmpty() || Data.WarningMessage.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ACommonHUD::CreateWarningPopup: Data is not valid."))
        return;
    }

    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonHUD::CreateWarningPopup: World is not valid."))
        return;
    }
    
    UJAFGCommonWidget* Widget = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WarningPopUpWidgetClass);
    Widget->PassDataToWidget(Data);
    Widget->AddToViewport();

    return;
}

void ACommonHUD::CreateWarningOptionPopUp(FWarningPopUpWidgetData& Data, const TFunction<void(bool bAccepted)>& OnOptionChosenDelegate) const
{
    if (Data.WarningHeader.IsEmpty() || Data.WarningMessage.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ACommonHUD::CreateWarningPopup: Data is not valid."))
        return;
    }

    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonHUD::CreateWarningPopup: World is not valid."))
        return;
    }

    Data.OnOptionChosenDelegate = OnOptionChosenDelegate;
    
    UJAFGCommonWidget* Widget = CreateWidget<UJAFGCommonWidget>(this->GetWorld(), this->WarningPopUpOptionWidgetClass);
    Widget->PassDataToWidget(Data);
    Widget->AddToViewport();

    return;
}
