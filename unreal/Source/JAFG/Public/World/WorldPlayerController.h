// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/JAFGPlayerController.h"

#include "WorldPlayerController.generated.h"

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerController : public AJAFGPlayerController
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay(void) override;
    void OnPostLogin(void);

private:

    UFUNCTION(Client, Reliable)
    void SetHyperlaneIdentifier_ClientRPC(const FString& InHyperlaneIdentifier);

    FString HyperlaneIdentifier = L"";

public:

    FORCEINLINE FString GetHyperlaneIdentifier(void) const
    {
        if (this->HyperlaneIdentifier.IsEmpty())
        {
            UE_LOG(LogTemp, Fatal, TEXT("AWorldPlayerController::GetHyperlaneIdentifier: Hyperlane Identifier has yet not been set."))
            return L"";
        }

        return this->HyperlaneIdentifier;
    }
};
