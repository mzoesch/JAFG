// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Network/NetworkStatics.h"
#include "Player/JAFGPlayerController.h"

#include "WorldPlayerController.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerController : public AJAFGPlayerController
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay(void) override;
    /** Must be called by the Game Mode in the server when this clients Post Login event has been fired. */
    void OnPostLogin(void);

private:

    /**
     * Server only.
     *
     * The address of the hyperlane worker that this player controller is connected to.
     */
    FString HyperlaneWorkerAddress = L"";
    /** Generated on server and replicated to the client on Post Login.  */
    FString HyperlaneIdentifier = L"";
    /** Also true on standalone games. */
    bool bListenServerController = false;
    UFUNCTION()
    void OnRep_IsConnectionEstablishedAndValidated() const;
    UPROPERTY(ReplicatedUsing=OnRep_IsConnectionEstablishedAndValidated)
    bool bIsConnectionEstablishedAndValidated = false;

    UFUNCTION(Client, Reliable)
    void SetHyperlaneIdentifier_ClientRPC(const FString& InHyperlaneIdentifier);

public:

    FORCEINLINE void SetHyperlaneWorkerAddress(const FString& InHyperlaneWorkerAddress)
    {
        if (UNetworkStatics::IsServer(this) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneWorkerAddress: Called on a client. This is disallowed."))
            return;
        }

        if (InHyperlaneWorkerAddress.IsEmpty())
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneWorkerAddress: Hyperlane Worker Address is empty."))
            return;
        }

        if (this->HyperlaneWorkerAddress.IsEmpty() == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneWorkerAddress: Hyperlane Worker Address has already been set. Has: [%s]. Got: [%s]."), *this->HyperlaneWorkerAddress, *InHyperlaneWorkerAddress)
            return;
        }

        this->HyperlaneWorkerAddress = InHyperlaneWorkerAddress;

        return;
    }

    FORCEINLINE FString GetHyperlaneWorkerAddress(void) const
    {
        if (UNetworkStatics::IsServer(this) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::GetHyperlaneWorkerAddress: Called on a client. This is disallowed."))
            return L"";
        }

         return this->HyperlaneWorkerAddress;
    }

    FORCEINLINE FString GetHyperlaneIdentifier(void) const
    {
        /*
         * We probably should move this to the callee and check if the string is empty there.
         */
        if (this->HyperlaneIdentifier.IsEmpty())
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::GetHyperlaneIdentifier: Hyperlane Identifier has yet not been set."))
            return L"";
        }

        return this->HyperlaneIdentifier;
    }

    FORCEINLINE void SetConnectionValidAndEstablished(void)
    {
        if (UNetworkStatics::IsServer(this) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetConnectionValidAndEstablished: Called on a client. This is disallowed."))
            return;
        }

        LOG_WARNING(LogTemp, "Connection established and validated. Setting replciated prop")

        this->bIsConnectionEstablishedAndValidated = true;

        return;
    }

    FORCEINLINE bool IsConnectionValidAndEstablished(void) const
    {
        if (UNetworkStatics::IsServer(this))
        {
            LOG_FATAL(LogTemp, "Called on server. This is disallowed.")
            return false;
        }

        return this->bIsConnectionEstablishedAndValidated;
    }
};
