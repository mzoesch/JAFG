// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"
#include "Lib/FAccumulated.h"

#include "UW_InventorySlot.generated.h"

class UImage;

UCLASS()
class JAFG_API UUW_InventorySlot : public UUW_Master
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* SlotBackground;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AccumulatedPreview;

public:

	void OnAccumulatedPreviewUpdate(const FAccumulated Accumulated) const;
};
