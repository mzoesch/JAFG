// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "Lib/FAccumulated.h"

#include "UW_InventorySlotV2.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UUW_InventorySlotV2 : public UUW_Master
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemPreview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ItemAmount;

public:

    void OnItemPreviewUpdate(const FAccumulated Accumulated) const;
};
