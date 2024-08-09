// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonChunk.h"

#include "ConvexChunk.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AConvexChunk : public ACommonChunk
{
    GENERATED_BODY()

public:

    explicit AConvexChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void GenerateProceduralMesh(void) override;
};
