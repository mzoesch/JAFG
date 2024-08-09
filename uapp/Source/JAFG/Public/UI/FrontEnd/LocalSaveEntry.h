// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGFocusableUserWidget.h"

#include "LocalSaveEntry.generated.h"

JAFG_VOID

class UTextBlock;

struct FPassedLocalSaveEntryData final : public FPassedFocusableWidgetData
{
    FString SaveName;
};

UCLASS(Abstract, Blueprintable)
class JAFG_API ULocalSaveEntry : public UJAFGFocusableUserWidget
{
    GENERATED_BODY()

public:

    explicit ULocalSaveEntry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true", OptionalWidget = "true"))
    UTextBlock* TB_SaveName = nullptr;

    UFUNCTION(BlueprintPure, Category = "JAFG|Focusable")
    FORCEINLINE FString GetSaveName( /* void */ ) const { return this->SaveName; }

    FString SaveName;
};
