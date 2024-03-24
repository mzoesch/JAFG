// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/JAFGCommonWidget.h"

#include "OnlineSessionEntry.generated.h"

class UTextBlock;

struct JAFG_API FOnlineSessionEntryData final : FMyPassData
{
    virtual ~FOnlineSessionEntryData() = default;

    uint16 OnlineSessionEntryIndex;
    FString OnlineSessionName;
};

UCLASS(Abstract, Blueprintable)
class JAFG_API UOnlineSessionEntry : public UJAFGCommonWidget
{
    GENERATED_BODY()

protected:

    FOnlineSessionEntryData EntryData;

    UPROPERTY(BlueprintReadOnly, Category = "Menu", meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_OnlineSessionName;

    UFUNCTION(BlueprintPure, Category = "Menu", meta = (AllowPrivateAccess = "true"))
    int GetEntryIndex() const { return this->EntryData.OnlineSessionEntryIndex; }

public:

    virtual void PassDataToWidget(const FMyPassData& MyPassedData) override;
};
