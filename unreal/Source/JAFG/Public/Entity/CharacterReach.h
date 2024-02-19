// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "CharacterReach.generated.h"

class UBoxComponent;

UCLASS()
class JAFG_API ACharacterReach : public AActor
{
    GENERATED_BODY()
    
public:

    ACharacterReach();

    virtual void BeginPlay(void) override;

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMesh* CharacterReachStaticMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UMaterialInterface* CharacterReachMaterial;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMaterialInstanceDynamic> MDynamicCharacterReachMaterial;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* Box;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Mesh;

public:

    void OnUpdate(const bool bVisible) const;
    /** Does not has to be a voxel location. The method will figure out the targeted voxel on its own. */
    void OnUpdate(const FVector& WorldLocation) const;
    /**
     * @param Progress The progress the current targeted voxel has
     *                 been minded (must be between zero (inclusive) and one (exclusive)).
     */
    void UpdateMaterial(const float Progress) const;
};
