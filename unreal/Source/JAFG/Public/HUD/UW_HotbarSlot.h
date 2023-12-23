// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "Lib/FAccumulated.h"

#include "UW_HotbarSlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class JAFG_API UUW_HotbarSlot : public UUW_Master
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* ItemPreview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ItemAmount;

public:

    void OnItemPreviewUpdate(const FAccumulated Accumulated) const;

public:

    /* TODO We ofc should not link the textures but search for them in the files! */

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TStoneVoxel;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TDirtVoxel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TGrassVoxel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TGlassVoxel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TLogVoxel;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TPlanksVoxel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* TLeavesVoxel;
    
};
