// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/Container/Container.h"

#include "PlayerInventory.generated.h"

class UTileView;
class UW_PlayerInventoryCrafterOutput;

UCLASS()
class JAFG_API UW_PlayerInventory : public UW_Container
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* CharacterInventoryCrafterSlots;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UW_PlayerInventoryCrafterOutput* CharacterInventoryCrafterOutput;
	
protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;

	virtual void RefreshCharacterInventory() override;
};
