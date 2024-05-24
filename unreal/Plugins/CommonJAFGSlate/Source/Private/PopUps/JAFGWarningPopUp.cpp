// Copyright 2024 mzoesch. All rights reserved.

#include "PopUps/JAFGWarningPopUp.h"

#include "Components/Button.h"

UJAFGWarningPopUp::UJAFGWarningPopUp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGWarningPopUp::NativeConstruct()
{
    Super::NativeConstruct();

    if (this->B_CloseButton)
    {
        this->B_CloseButton->OnClicked.AddDynamic(this, &UJAFGWarningPopUp::OnClose);
    }

    if (this->TB_Header)
    {
        this->TB_Header->SetText(FText::FromString(this->Header));
    }

    if (this->TB_Message)
    {
        this->TB_Message->SetText(FText::FromString(this->Message));
    }

    return;
}

void UJAFGWarningPopUp::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FJAFGWarningPopUpData)
    {
        this->Header  = Data->Header;
        this->Message = Data->Message;
    }

    return;
}

FString UJAFGWarningPopUp::GetHeader(void) const
{
    return this->Header;
}

FString UJAFGWarningPopUp::GetMessage(void) const
{
    return this->Message;
}

void UJAFGWarningPopUp::OnClose(void)
{
    this->RemoveFromParent();
}
