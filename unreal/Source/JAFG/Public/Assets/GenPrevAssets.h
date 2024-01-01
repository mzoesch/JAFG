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

private:

	bool bIsGenerated = false;
	bool bIsRendered = false;
	int waitCount = 0;
	
};
