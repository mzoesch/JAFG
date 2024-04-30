// Copyright 2024 mzoesch. All rights reserved.

#include "SubMenus/Entries/LocalSaveEntry.h"

#include "Components/TextBlock.h"

void ULocalSaveEntry::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FLocalSaveEntryData* LocalSaveSlotData = static_cast<const FLocalSaveEntryData*>(&MyPassedData); LocalSaveSlotData == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FLocalSaveEntryData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FLocalSaveEntryData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->EntryData = *LocalSaveSlotData;
    }

    this->TB_SaveName->SetText(FText::FromString(this->EntryData.SaveEntryName));

    this->OnDeferredConstruct();

    return;
}
