// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/OnlineSessionEntry.h"

#include "Components/TextBlock.h"

void UOnlineSessionEntry::PassDataToWidget(const FMyPassData& MyPassedData)
{
    if (const FOnlineSessionEntryData* OnlineSessionEntryData = static_cast<const FOnlineSessionEntryData*>(&MyPassedData); OnlineSessionEntryData == nullptr)
    {
#if WITH_EDITOR
        UE_LOG(LogTemp, Error, TEXT("UOnlineSessionEntry::PassDataToWidget: MyPassedData is not of type FOnlineSessionEntryData."))
#else
        UE_LOG(LogTemp, Fatal, TEXT("UOnlineSessionEntry::PassDataToWidget: MyPassedData is not of type FOnlineSessionEntryData."))
#endif /* WITH_EDITOR */
    }
    else
    {
        this->EntryData = *OnlineSessionEntryData;
    }

    this->TB_OnlineSessionName->SetText(FText::FromString(this->EntryData.GetSanitizedSessionName()));
    this->TB_OnlineSessionConnections->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), this->EntryData.GetCurrentPublicConnections(), this->EntryData.MaxPublicConnections)));

    this->OnDeferredConstruct();

    return;
}
