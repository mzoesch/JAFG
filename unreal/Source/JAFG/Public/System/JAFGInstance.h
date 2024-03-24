// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "JAFGInstance.generated.h"

class ULocalSessionSupervisorSubsystem;

UCLASS()
class JAFG_API UJAFGInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    
    virtual void Init() override;
    virtual void Shutdown() override;
};
