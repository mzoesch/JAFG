// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGExternalCoreIncludes.h"
#include "ExternalGameInstanceSubsystem.h"

#include "FontSubsystem.generated.h"

JAFG_VOID

class UFontFace;

UENUM(BlueprintType)
namespace EJAFGFontTypeFace
{

enum Type : uint8
{
    Default,
    Bold,
    Italic,
    BoldItalic,
};

}


USTRUCT(NotBlueprintable, NotBlueprintType)
struct JAFGEXTERNALCORE_API FJAFGFontData
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UFont> Font = nullptr;

    UPROPERTY()
    TObjectPtr<UFontFace> FontFace_Default    = nullptr;

    UPROPERTY()
    TObjectPtr<UFontFace> FontFace_Bold       = nullptr;

    UPROPERTY()
    TObjectPtr<UFontFace> FontFace_Italic     = nullptr;

    UPROPERTY()
    TObjectPtr<UFontFace> FontFace_BoldItalic = nullptr;

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

    /**
     * The core font. It requires a default type face, a bold one, an italic one, and a bold italic one.
     */
    UPROPERTY()
    FJAFGFontData CoreFontData;

    static auto GetTypeByScheme(const EJAFGFontTypeFace::Type InScheme) -> FName;

private:

    bool LoadFontFile(const FString& FileName, TArray<uint8>& OutBytes, TArray<FString> AllowedExtensions = {TEXT(".otf"), TEXT(".ttf")}) const;
};
