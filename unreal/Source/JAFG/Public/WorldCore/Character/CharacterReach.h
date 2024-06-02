// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/Actor.h"

#include "CharacterReach.generated.h"

JAFG_VOID

class UBoxComponent;

UCLASS(NotBlueprintable)
class JAFG_API ACharacterReach : public AActor
{
    GENERATED_BODY()

public:

    explicit ACharacterReach(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componets", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBoxComponent> BoxComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componets", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componets", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMaterialInstanceDynamic> DynamicDestructionMaterial;

public:

    void Update(const bool bVisible) const;
    void Update(const FVector& WorldLocation) const;
    /** @param Progress Value range [0, 1]. */
    void Update(float Progress) const;
};
