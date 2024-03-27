// Copyright 2024 mzoesch. All rights reserved.

#include "Network/ChatComponent.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "UI/Common/CommonHUD.h"
#include "UI/World/WorldHUD.h"

UChatComponent::UChatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UChatComponent::BeginPlay(void)
{
    Super::BeginPlay();
}

void UChatComponent::QueueMessage(const FString& Message)
{
    this->QueueMessage_ServerRPC(Message);
}

bool UChatComponent::QueueMessage_ServerRPC_Validate(const FString& Message)
{
    return Message.Len() > 0 && Message.Len() <= UChatComponent::MaxChatInputLength;
}

void UChatComponent::QueueMessage_ServerRPC_Implementation(const FString& Message)
{
    const int32 PredictedNumPlayers = this->GetWorld()->GetNumPlayerControllers();
    
    UE_LOG(LogTemp, Log, TEXT("AChatInfo::QueueMessage_ServerRPC: Received [%s]. Pening clients: %d."), *Message, PredictedNumPlayers)
    
    if (PredictedNumPlayers != this->GetWorld()->GetGameState()->PlayerArray.Num())
    {
        UE_LOG(LogTemp, Fatal, TEXT("AChatInfo::QueueMessage_ServerRPC: Player count mismatch: PC: %d != PS: %d."), PredictedNumPlayers, this->GetWorld()->GetGameState()->PlayerArray.Num())
        return;
    }
    
    for (int i = 0; i < PredictedNumPlayers; ++i)
    {
        const APlayerState* PlayerState = this->GetWorld()->GetGameState()->PlayerArray[i];

        if (PlayerState == nullptr)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AChatInfo::QueueMessage_ServerRPC: PlayerState at index [%d] is invalid."), i)
            return;;
        }

        UChatComponent* Target = PlayerState->GetPawn()->FindComponentByClass<UChatComponent>();

        if (Target == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("AChatInfo::QueueMessage_ServerRPC: Target is invalid."))
            continue;
        }

        Target->AddMessageToChat_ClientRPC(Message);

        continue;
    }

    return;
}

void UChatComponent::AddMessageToChat_ClientRPC_Implementation(const FString& Message)
{
    Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD())->AddMessageToChat(Message);
    
}
