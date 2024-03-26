// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/Common/JAFGCommonWidget.h"

#include "LocalSaveEntry.generated.h"

class UTextBlock;

/**
 * Holds relevant data for a local save slot that the User Interface can use and display.
 * Does not hold the actual saved data and is just a reference to a local saved data slot folder.
 */
struct JAFG_API FLocalSaveEntryData final : FMyPassData
{
    virtual ~FLocalSaveEntryData() = default;

    uint16  SaveEntryIndex;
    FString SaveEntryName;

    inline virtual FString ToString() const override
    {
        return FString::Printf(TEXT("FLocalSaveSlotData{SaveEntryIndex: %d, SaveEntryName: %s}"), this->SaveEntryIndex, *this->SaveEntryName);
    }
};

UCLASS(Abstract, Blueprintable)
class JAFG_API ULocalSaveEntry : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    FLocalSaveEntryData EntryData;
    
    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_SaveName;

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int GetEntryIndex() const { return this->EntryData.SaveEntryIndex; }
    
public:
    
    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
};
