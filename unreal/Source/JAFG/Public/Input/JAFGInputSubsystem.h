// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "JAFGInputSubsystem.generated.h"

class UInputAction;
class UInputMappingContext;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UJAFGInputSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    TObjectPtr<UInputMappingContext> MyInputMappingContext;
    TObjectPtr<UInputAction> MyInputAction;
};
