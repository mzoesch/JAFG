// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Lib/FAccumulated.h"

#include "Drop.generated.h"

class ACuboid;

UCLASS()
class JAFG_API ADrop : public AActor
{
	GENERATED_BODY()
	
public:	

	ADrop();
	explicit ADrop(const FAccumulated InAccumulated);
	
protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY()
	ACuboid* Cuboid;

private:

	FAccumulated Accumulated;

public:

	FORCEINLINE void SetAccumulated(const FAccumulated InAccumulated) { this->Accumulated = InAccumulated; }

	void AddForce(const FVector& Force) const;

private:

	void RenderMesh() const;
	
};
