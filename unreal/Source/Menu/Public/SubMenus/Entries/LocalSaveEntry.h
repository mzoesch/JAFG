// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "FrontEnd/JAFGWidget.h"

#include "LocalSaveEntry.generated.h"

JAFG_VOID

class UTextBlock;

/**
 * Holds relevant data for a local save slot that the User Interface can use and display.
 * Does not hold the actual saved data and is just a reference to a local saved data slot folder.
 */
struct MENU_API FLocalSaveEntryData final : FMyPassedData
{
    virtual ~FLocalSaveEntryData(void) = default;

    uint16  SaveEntryIndex;
    FString SaveEntryName;

    FORCEINLINE virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("FLocalSaveSlotData{SaveEntryIndex: %d, SaveEntryName: %s}"), this->SaveEntryIndex, *this->SaveEntryName);
    }
};

UCLASS(Abstract, Blueprintable)
class MENU_API ULocalSaveEntry : public UJAFGWidget
{
    GENERATED_BODY()

protected:

    FLocalSaveEntryData EntryData;

    UPROPERTY(BlueprintReadOnly, Category = "JAFG|Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_SaveName;

    UFUNCTION(BlueprintPure, Category = "JAFG|Menu", meta = (AllowPrivateAccess = "true"))
    int GetEntryIndex( /* void */ ) const { return this->EntryData.SaveEntryIndex; }

public:

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;
};
