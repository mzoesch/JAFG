// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/LocalSaveEntry.h"

#include "Components/TextBlock.h"

void ULocalSaveEntry::PassDataToWidget(const FMyPassData& MyPassedData)
{
    if (const FLocalSaveEntryData* LocalSaveSlotData = static_cast<const FLocalSaveEntryData*>(&MyPassedData); LocalSaveSlotData == nullptr)
    {
#if WITH_EDITOR
        UE_LOG(LogTemp, Error, TEXT("ULocalSaveEntry::PassDataToWidget: MyPassedData is not of type FLocalSaveEntryData."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("ULocalSaveEntry::PassDataToWidget: MyPassedData is not of type FLocalSaveEntryData."))
#endif /* WITH_EDITOR */
        return;
    }
    else
    {
        this->EntryData = *LocalSaveSlotData;
    }

    this->TB_SaveName->SetText(FText::FromString(this->EntryData.SaveEntryName));

    return;
}
