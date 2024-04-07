// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldPlayerController.h"

#include "Network/NetworkStatics.h"

AWorldPlayerController::AWorldPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldPlayerController::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeServer(this))
    {
        /* Replicated after client has connected in post login.  */
        this->HyperlaneIdentifier = FGuid::NewGuid().ToString();
        UE_LOG(LogTemp, Warning, TEXT("AWorldPlayerController::BeginPlay: Creating new GUID for %s: [%s]."), *this->GetName(), *this->HyperlaneIdentifier)
    }

    return;
}

void AWorldPlayerController::OnPostLogin()
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::OnPostLogin called on client. This is disallowed."))
        return;
    }

    this->SetHyperlaneIdentifier_ClientRPC(this->HyperlaneIdentifier);

    return;
}

void AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC_Implementation(const FString& InHyperlaneIdentifier)
{
    UE_LOG(LogTemp, Warning, TEXT("AWorldPlayerController::SetHyperlaneIdentifier_ClientRPC: Got replicated Hyperlane Identifier: [%s]."), *InHyperlaneIdentifier)

    if (this->HyperlaneIdentifier.IsEmpty())
    {
        this->HyperlaneIdentifier = InHyperlaneIdentifier;
    }
    else
    {
        UE_LOG(LogTemp, Fatal, TEXT("Hyperlane identifier already set: [%s]. But received: [%s]."), *this->HyperlaneIdentifier, *InHyperlaneIdentifier)
    }

    return;
}
