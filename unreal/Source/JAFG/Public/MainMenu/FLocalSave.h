// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FLocalSave.generated.h"


USTRUCT(BlueprintType)
struct JAFG_API FLocalSave {
public:
	GENERATED_BODY()
	
public:

	FLocalSave();
	FLocalSave(FString SaveName, FString LastPlayedDate);
	~FLocalSave();

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString SaveName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString LastPlayedDate;
	
};
