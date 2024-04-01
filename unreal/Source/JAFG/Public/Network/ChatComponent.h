// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ChatComponent.generated.h"

UCLASS(ClassGroup=(Custom))
class JAFG_API UChatComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    inline static constexpr int MaxChatInputLength = 0x3F; 

    explicit UChatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay() override;

public:

    FORCEINLINE virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
    { UE_LOG(LogTemp, Fatal, TEXT("UChatComponent::Tick")); }

    void QueueMessage(const FString& Message);

private:

    UFUNCTION(Server, Reliable, WithValidation)
    void QueueMessage_ServerRPC(const FString& Message);

    UFUNCTION(Client, Reliable)
    void AddMessageToChat_ClientRPC(const FString& Message);
};
