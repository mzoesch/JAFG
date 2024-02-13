// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Drop.generated.h"

class ACuboid;

UCLASS()
class JAFG_API ADrop : public AActor
{
	GENERATED_BODY()
	
public:	

	ADrop();
	explicit ADrop(const int V);
	
protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneComponent;

	UPROPERTY()
	class ACuboid* Cuboid;

private:

	int Voxel;

public:

	void SetVoxel(const int V);

private:

	void RenderMesh() const;
	
};
