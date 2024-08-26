// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGExternalCoreIncludes.h"
#include "ExternalGameInstanceSubsystem.h"

#include "FontSubsystem.generated.h"

JAFG_VOID

class UFontFace;

USTRUCT(NotBlueprintable, NotBlueprintType)
struct JAFGEXTERNALCORE_API FJAFGFontData
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UFontFace> FontFace = nullptr;

    UPROPERTY()
    TObjectPtr<UFont> Font         = nullptr;

    /**
     * Be aware of your enemy, the Garbage Collector.
     * You are the owner; Store the pointer somewhere safe if needed.
     */
    auto ConstructNewMinimalDataTable(void) const -> UDataTable*;
    auto ConstructCompositeFont(void) const -> TSharedPtr<FCompositeFont>;
};

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UFontSubsystem : public UExternalGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UFontSubsystem();

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation

    FJAFGFontData CoreFontData;
};
