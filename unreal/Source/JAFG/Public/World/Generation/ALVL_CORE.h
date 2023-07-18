// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/Generation/FChunk.h"

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

	TArray<FChunk> LoadedChunks;

public:

	UFUNCTION(BlueprintCallable)
	void UpdateLoadedChunks(bool bForceUpdate);
	static const inline float WAIT_FOR_NEXT_CHUNK_UPDATE = 0.01f;
	float TimeSinceLastChunkUpdate = 0.0f;

	UFUNCTION(Server, Unreliable)
	void SV_LoadChunkToWorld(const FChunk& Chunk);
	void BalanceLoadedChunks();

// Temp
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> TempChunkActorClass;

};
