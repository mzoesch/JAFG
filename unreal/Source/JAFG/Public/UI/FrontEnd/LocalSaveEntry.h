// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGFocusableWidget.h"

#include "LocalSaveEntry.generated.h"

JAFG_VOID

class UTextBlock;

struct FPassedLocalSaveEntryData final : public FPassedFocusableWidgetData
{
    FString SaveName;

    FORCEINLINE virtual FString ToString(void) const override
    {
        return FString::Printf(TEXT("FPassedLocalSaveEntryData{SaveName:%s}"), *this->SaveName);
    }
};

UCLASS(Abstract, Blueprintable)
class JAFG_API ULocalSaveEntry : public UJAFGFocusableWidget
{
    GENERATED_BODY()

public:

    explicit ULocalSaveEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FMyPassedData& MyPassedData) override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UTextBlock* TB_SaveName = nullptr;

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable")
    FORCEINLINE FString GetSaveName( /* void */ ) const { return this->SaveName; }

    FString SaveName;
};
