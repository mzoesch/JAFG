// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/LocalSaveEntry.h"

#include "Components/TextBlock.h"

ULocalSaveEntry::ULocalSaveEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ULocalSaveEntry::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FPassedLocalSaveEntryData)
    {
        this->SaveName = Data->SaveName;
    }

    if (this->TB_SaveName)
    {
        this->TB_SaveName->SetText(FText::FromString(this->SaveName));
    }

    return;
}
