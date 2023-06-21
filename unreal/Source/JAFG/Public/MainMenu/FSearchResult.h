// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FSearchResult.generated.h"


USTRUCT(BlueprintType)
struct JAFG_API FSearchResult {
public:
	GENERATED_BODY()

public:

	FSearchResult();
	~FSearchResult();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SessionName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxPublicConnections;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentPublicConnections;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PingInMs;

};
