// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "HUD/UW_Master.h"

#include "Lib/FAccumulated.h"

#include "UW_InventorySlotV2.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UUW_InventorySlotV2 : public UUW_Master, public IUserObjectListEntry
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemPreview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ItemAmount;

public:

    void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    
    void OnItemPreviewUpdate(const FAccumulated Accumulated) const;
};
