// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Voxel.h"

#include "GrassVoxel.generated.h"

class UGI_Master;

UCLASS()
class JAFG_API UGrassVoxel : public UObject, public IVoxel
{
	GENERATED_BODY()

public:

	virtual void Initialize(class UGI_Master* GI) override;
	
};
