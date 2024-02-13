// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Voxel.h"

#include "DirtVoxel.generated.h"

class UGI_Master;

UCLASS()
class JAFG_API UDirtVoxel : public UObject, public IVoxel
{
	GENERATED_BODY()

public:

	virtual void Initialize(class UGI_Master* GI) override;
};
