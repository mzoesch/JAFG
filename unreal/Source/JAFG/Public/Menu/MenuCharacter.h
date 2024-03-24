// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MenuCharacter.generated.h"

class UJAFGFrontEnd;

UCLASS(Abstract, Blueprintable)
class JAFG_API AMenuCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AMenuCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<UJAFGFrontEnd> FrontEndClass;

protected:

    virtual void BeginPlay() override;
    
private:

    void DisplayMenu() const;
};
