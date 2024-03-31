// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MaterialSubsystem.generated.h"

namespace ETextureGroup
{

enum Type : int8
{
	Core = -1,
	Opaque,
};

}

UCLASS(NotBlueprintable)
class JAFG_API UMaterialSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	inline static constexpr int TextureArrayWidthHorizontal { 16 };
	inline static constexpr int TextureArrayWidthVertical   { 16 };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials", meta = (ToolTip = "Automatically set by the subsystem."))
	TObjectPtr<UMaterialInstanceDynamic> MDynamicOpaque;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize(void) override;

private:
	
	void InitializeMaterials(void);
};
