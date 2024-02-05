// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "UW_AccumulatedCursorPreview.generated.h"

class UImage;

UCLASS()
class JAFG_API UUW_AccumulatedCursorPreview : public UUW_Master
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AccumulatedPreview;
	
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;
	
	void OnUpdate();
};
