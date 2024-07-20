// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSetting.h"

#include "CustomSettingsLocalPlayer.h"
#include "JAFGLogDefs.h"

UGameSetting::UGameSetting(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSetting::Initialize(UCustomSettingsLocalPlayer* InOwningPlayer)
{
    if (InOwningPlayer == nullptr || !InOwningPlayer->IsValidLowLevel())
    {
        LOG_FATAL(LogGameSettings, "Invalid Owning Player received.")
        return;
    }

    if (this->OwningPlayer != nullptr)
    {
        LOG_FATAL(LogGameSettings, "Setting already initialized.")
        return;
    }

    this->OwningPlayer = InOwningPlayer;

    if (this->Identifier.IsEmpty())
    {
        LOG_FATAL(LogGameSettings, "Identifier is empty but was initialzied.")
        return;
    }

    if (this->DisplayName.IsEmpty())
    {
        LOG_FATAL(LogGameSettings, "Display Name is empty but was initialzied.")
        return;
    }

    for (UGameSetting* Setting : this->GetChildSettings())
    {
        Setting->Initialize(InOwningPlayer);
    }

    this->OnInitialized();

    return;
}
