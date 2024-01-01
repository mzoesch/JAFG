// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GenPrevAssets.generated.h"

class USceneComponent;
class USceneCaptureComponent2D;

UCLASS()
class JAFG_API AGenPrevAssets : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGenPrevAssets();

public:
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	class USceneCaptureComponent2D* CaptureComponent;
	
protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(const float DeltaTime) override;

	// (Rotation=(X=0.898527,Y=0.321476,Z=0.276089,W=0.114360),Translation=(X=620.000000,Y=0.000000,Z=15.000000),Scale3D=(X=1.000000,Y=1.000000,Z=1.000000))
	UPROPERTY(EditAnywhere)
	FTransform Transform = FTransform(FRotator(180, 45, 0), FVector(500, 0, -200), FVector::OneVector);
	
private:

	FTransform PrevTransform = FTransform(FQuat(0.898527, 0.321476, 0.276089, 0.114360), FVector(620, 0, 15), FVector::OneVector);
	
};
