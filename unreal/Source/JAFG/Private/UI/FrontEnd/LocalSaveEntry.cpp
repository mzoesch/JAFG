// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/LocalSaveEntry.h"

#include "Components/TextBlock.h"

ULocalSaveEntry::ULocalSaveEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ULocalSaveEntry::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    Super::PassDataToWidget(MyPassedData);

    if (const FPassedLocalSaveEntryData* Data = static_cast<const FPassedLocalSaveEntryData*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FPassedLocalSaveEntryData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FPassedLocalSaveEntryData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->SaveName = Data->SaveName;
    }

    if (this->TB_SaveName)
    {
        this->TB_SaveName->SetText(FText::FromString(this->SaveName));
    }

    this->OnDeferredConstruct();

    return;
}
