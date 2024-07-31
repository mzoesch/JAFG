// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ModificationUBTEditorToolSettings.generated.h"

UCLASS(config=JAFGEditor)
class MODIFICATIONUBTEDITORTOOL_API UModificationUBTEditorToolSettings : public UObject
{
    GENERATED_BODY()

public:

    FORCEINLINE static auto Get(void) -> UModificationUBTEditorToolSettings*
    {
        return GetMutableDefault<UModificationUBTEditorToolSettings>();
    }

    void SaveSettings(void);
};
