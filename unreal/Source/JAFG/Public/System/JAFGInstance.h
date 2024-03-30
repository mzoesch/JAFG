// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "JAFGInstance.generated.h"

class ULocalSessionSupervisorSubsystem;

UCLASS(Abstract, Blueprintable)
class JAFG_API UJAFGInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    //////////////////////////////////////////////////////////////////////////
    // Materials
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditDefaultsOnly, Category = "Materials")
    const TObjectPtr<UMaterialInterface> MOpaque;

    // TEMP

    UPROPERTY(EditDefaultsOnly, Category="Material")
    TObjectPtr<UTexture2D> Stone;
    
    UPROPERTY(EditDefaultsOnly, Category="Material")
    TObjectPtr<UTexture2D> Dirt;
    
    UPROPERTY(EditDefaultsOnly, Category="Material")
    TObjectPtr<UTexture2D> GrassTop;
    
    UPROPERTY(EditDefaultsOnly, Category="Material")
    TObjectPtr<UTexture2D> Grass;
    
    virtual void Init(void) override;
    virtual void Shutdown(void) override;
};
