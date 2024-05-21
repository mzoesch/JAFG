// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/RemoteSessionEntry.h"

#include "Components/TextBlock.h"

URemoteSessionEntry::URemoteSessionEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void URemoteSessionEntry::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    Super::PassDataToWidget(MyPassedData);

    if (const FPassedRemoteSessionEntryData* Data = static_cast<const FPassedRemoteSessionEntryData*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FPassedRemoteSessionEntryData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FPassedRemoteSessionEntryData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->EntryData = *Data;
    }

    if (this->TB_SessionName)
    {
        this->TB_SessionName->SetText(FText::FromString(this->EntryData.GetDisplaySessionName()));
    }

    if (this->TB_SessionPing)
    {
        this->TB_SessionPing->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), this->EntryData.PingInMs)));
    }

    if (this->TB_SessionPlayers)
    {
        this->TB_SessionPlayers->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), this->EntryData.GetCurrentPublicConnections(), this->EntryData.NumPublicConnections)));
    }

    this->OnDeferredConstruct();

    return;
}
