// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldPlayerController.h"

#include "Misc/CommonLogging.h"
#include "Net/UnrealNetwork.h"
#include "Network/NetworkStatics.h"

AWorldPlayerController::AWorldPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bReplicates = true;

    return;
}

void AWorldPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWorldPlayerController, bIsConnectionEstablishedAndValidated);

    return;
}

void AWorldPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeServer(this))
    {
        /* Replicated after client has connected in Post Login.  */
        this->HyperlaneIdentifier = FGuid::NewGuid().ToString();
        UE_LOG(LogTemp, Display, TEXT("AWorldPlayerController::BeginPlay: Creating new GUID for %s: [%s]."), *this->GetName(), *this->HyperlaneIdentifier)
    }

    /*
     * Post Login on a Host Player Controller of any listen server will, for whatever reason, be called prior by the
     * Engine to the Game Mode than the Begin Play of the Player Controller???
     * If this is not a host it is the direct opposite. We first initialize the Player Controller and then the Post
     * Login is called by the Engine in the Game Mode.
     */
    if (UNetworkStatics::IsSafeListenServer(this) && this->IsLocalController())
    {
        if (this->bListenServerController == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::BeginPlay: Listen Server Controller flag not set."))
            return;
        }

        /*
         * This should be and is not wrong.
         * The host of this listen server will not send a worker to the Hyperlane. We always mock the worker for
         * the host of a listen server.
         * Remote Procedure Calls will also not work from the host to the host. Therefore, we call the implementation
         * directly and not through unreals RPC system.
         */
        this->SetHyperlaneIdentifier_ClientRPC_Implementation(this->HyperlaneIdentifier);

        return;
    }

    return;
}

void AWorldPlayerController::OnPostLogin(void)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::OnPostLogin called on client. This is disallowed."))
        return;
    }

    if (UNetworkStatics::IsSafeListenServer(this) && this->IsLocalController() || UNetworkStatics::IsSafeStandalone(this))
    {
        /*
         * Because the post login will be called before the Begin Play if this player controller is the host of any
         * listen server.
         * That is why we move the Hyperlane Identifier replication to the Begin Play. See there for more information.
         */
        this->bListenServerController = true;

        if (this->HyperlaneIdentifier.IsEmpty() == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::OnPostLogin: Hyperlane Identifier already set on a listen server host player controller."))
            return;
        }

        UE_LOG(LogTemp, Verbose, TEXT("AWorldPlayerController::OnPostLogin: Not setting Hyerlane Identifier on [%s] as it is the host listen server player controller. Moving logic to Begin Play."), *this->GetName())

        return;
    }

    this->SetHyperlaneIdentifier_ClientRPC(this->HyperlaneIdentifier);

    return;
}

void AWorldPlayerController::OnRep_IsConnectionEstablishedAndValidated(void) const
{
    if (UNetworkStatics::IsSafeServer(this))
    {
        LOG_FATAL(LogTemp, "Called on server. This is disallowed.")
        return;
    }

    LOG_WARNING(LogTemp, "Connection established and validated. %hhd", this->bIsConnectionEstablishedAndValidated)

    return;
}

void AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC_Implementation(const FString& InHyperlaneIdentifier)
{
    UE_LOG(LogTemp, Display, TEXT("AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC: Got replicated Hyperlane Identifier: [%s]."), *InHyperlaneIdentifier)

    if (InHyperlaneIdentifier.IsEmpty())
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC: In Hyperlane Identifier is empty."))
        return;
    }

    if (this->HyperlaneIdentifier.IsEmpty())
    {
        this->HyperlaneIdentifier = InHyperlaneIdentifier;
    }
    else
    {
        if (this->bListenServerController)
        {
            if (this->HyperlaneIdentifier == InHyperlaneIdentifier)
            {
                return;
            }

            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC: Hyperlane Identifier is not the same as the In RPC Hyperlane Indentifier on the listen server player controller host. Has: [%s]. Got: [%s]."), *this->HyperlaneIdentifier, *InHyperlaneIdentifier)
            return;
        }

        UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC: Hyperlane identifier already set: [%s]. But received: [%s]."), *this->HyperlaneIdentifier, *InHyperlaneIdentifier)
        return;
    }

    return;
}
