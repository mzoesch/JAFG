// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Lib/Item.h"
#include "World/Voxel.h"

#include "Drop.generated.h"

class ACuboid;

UCLASS()
class JAFG_API ADrop : public AActor
{
	GENERATED_BODY()
	
public:	

	ADrop();
	explicit ADrop(const EItem& I);
	explicit ADrop(const EVoxel& V);
	
protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneComponent;

	UPROPERTY()
	class ACuboid* Cuboid;

private:

	EItem Item;
	EVoxel Voxel;

public:

	void SetItem(const EItem& I);
	void SetVoxel(const EVoxel& V);

private:

	void RenderMesh() const;
	
};
