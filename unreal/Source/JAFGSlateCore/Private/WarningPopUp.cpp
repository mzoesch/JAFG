// Copyright 2024 mzoesch. All rights reserved.

#include "WarningPopUp.h"

#include "Components/Button.h"

UWarningPopUp::UWarningPopUp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->B_CloseButton = nullptr;
    return;
}

void UWarningPopUp::NativeConstruct(void)
{
    Super::NativeConstruct();

    if (this->B_CloseButton)
    {
        this->B_CloseButton->OnClicked.AddDynamic(this, &UWarningPopUp::OnClose);
    }

    return;
}

void UWarningPopUp::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FWarningPopUpData* Data = static_cast<const FWarningPopUpData*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FWarningPopUpData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FWarningPopUpData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->Header  = Data->Header;
        this->Message = Data->Message;
    }

    this->OnDeferredConstruct();

    return;
}

FString UWarningPopUp::GetHeader(void) const
{
    return this->Header;
}

FString UWarningPopUp::GetMessage(void) const
{
    return this->Message;
}

void UWarningPopUp::OnClose(void)
{
    this->RemoveFromParent();
}
