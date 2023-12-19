// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "GI_Master.generated.h"

UCLASS()
class JAFG_API UGI_Master : public UGameInstance
{
	GENERATED_BODY()

#pragma region Materials
	
public:

	UPROPERTY(EditDefaultsOnly, Category="Material")
	const TObjectPtr<UMaterialInterface> DevMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	const TObjectPtr<UMaterialInterface> TranslucentMaterial;

#pragma endregion Materials
	
};
