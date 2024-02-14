// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GenPrevAssets.generated.h"

class USceneComponent;
class USceneCaptureComponent2D;

/** Generates the preview assets textures for voxels. */
UCLASS()
class JAFG_API AGenPrevAssets : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGenPrevAssets();

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* CaptureComponent;
	
protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(const float DeltaTime) override;
	
private:

	void FormatGenPrevTextures();
	
	int PrevIndex;
	FTransform PrevTransform = FTransform(FQuat(0.898527, 0.321476, 0.276089, 0.114360), FVector(620, 0, 15), FVector::OneVector);
};
