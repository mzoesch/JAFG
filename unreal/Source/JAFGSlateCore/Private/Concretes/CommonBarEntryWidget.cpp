// Copyright 2024 mzoesch. All rights reserved.

#include "Concretes/CommonBarEntryWidget.h"

UCommonBarEntryWidget::UCommonBarEntryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UCommonBarEntryWidget::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FPassedTabDescriptor* Data = static_cast<const FPassedTabDescriptor*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FPassedTabDescriptor.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FPassedTabDescriptor.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
#if !UE_BUILD_SHIPPING
        if (Data->Owner == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Data->Owner is invalid.")
            return;
        }
#endif /* !UE_BUILD_SHIPPING */

        this->Owner         = Data->Owner;
        this->TabDescriptor = Data->Descriptor;
        this->InitializeTab();
    }

    this->OnDeferredConstruct();

    return;
}

void UCommonBarEntryWidget::OnThisTabPressed(void) const
{
    this->Owner->OnTabPressed(this->TabDescriptor.Identifier);
}

void UCommonBarEntryWidget::OnTabPressed(const FString& Identifier)
{
    if (Identifier == this->TabDescriptor.Identifier)
    {
        this->OnFocusTab();
    }
    else
    {
        this->OnUnfocusTab();
    }

    return;
}

FString UCommonBarEntryWidget::GetTabDisplayName(void) const
{
    return this->TabDescriptor.DisplayName;
}

void UCommonBarEntryWidget::InitializeTab(void)
{
    this->Owner->OnTabPressedEvent.AddUObject(this, &UCommonBarEntryWidget::OnTabPressed);
}
