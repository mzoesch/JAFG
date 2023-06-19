// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UJAFG_Widget.h"

#include "UJAFG_Widget_HotBarSlot.generated.h"


UCLASS()
class JAFG_API UJAFG_Widget_HotBarSlot : public UJAFG_Widget {
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* ItemTexture = nullptr;

	void SetItemTexture(UTexture2D* NewItemTexture);

};
