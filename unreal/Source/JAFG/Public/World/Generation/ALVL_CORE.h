// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ALVL_CORE.generated.h"


UCLASS()
class JAFG_API ALVL_CORE : public AActor {
	GENERATED_BODY()
	
public:	
	
	ALVL_CORE();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	TArray<FTransform> UpdateLoadedChunks();

};
