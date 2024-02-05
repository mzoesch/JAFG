// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "UW_AccumulatedCursorPreview.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;
class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UUW_AccumulatedCursorPreview : public UUW_Master
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AccumulatedPreview;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AccumulatedAmount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* AccumulatedWrapper;

private:
	
	UPROPERTY()
	UCanvasPanelSlot* AccumulatedWrapperSlot;
	
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;
	
	void OnUpdate();
};
