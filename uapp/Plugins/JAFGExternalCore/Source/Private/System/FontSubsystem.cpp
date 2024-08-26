// Copyright 2024 mzoesch. All rights reserved.

#include "System/FontSubsystem.h"
#include "Components/RichTextBlock.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "System/PathFinder.h"

#define FONT_FILE_NAME_DEFAULT     TEXT("Core")
#define FONT_FILE_NAME_BOLD        TEXT("Core_Bold")
#define FONT_FILE_NAME_ITALIC      TEXT("Core_Italic")
#define FONT_FILE_NAME_BOLD_ITALIC TEXT("Core_BoldItalic")

UDataTable* FJAFGFontData::ConstructNewMinimalDataTable(void) const
{
    FSlateFontInfo FontInfo;
    FontInfo.CompositeFont = MakeShared<FCompositeFont>(this->Font->CompositeFont);

    FTextBlockStyle DefaultStyle;
    DefaultStyle.Font = FontInfo;

    FRichTextStyleRow DefaultRow;
    DefaultRow.TextStyle = DefaultStyle;

    UDataTable* DataTable = NewObject<UDataTable>();
    DataTable->RowStruct = FRichTextStyleRow::StaticStruct();
    DataTable->AddRow(FName(TEXT("Default")), DefaultRow);

    return DataTable;
}

TSharedPtr<FCompositeFont> FJAFGFontData::ConstructCompositeFont(void) const
{
    return MakeShared<FCompositeFont>(this->Font->CompositeFont);
}

UFontSubsystem::UFontSubsystem(void) : Super()
{
    return;
}

void UFontSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->CoreFontData = FJAFGFontData();

    //////////////////////////////////////////////////////////////////////////
    // Load the core font data from disk
    {
        TArray<uint8> RawFontData;
        if (this->LoadFontFile(FONT_FILE_NAME_DEFAULT, RawFontData) == false)
        {
            LOG_FATAL(LogFontSubsystem, "Failed to load raw default core font data.")
            return;
        }
        this->CoreFontData.FontFace_Default = NewObject<UFontFace>();
        this->CoreFontData.FontFace_Default->LoadingPolicy = EFontLoadingPolicy::Inline;
        this->CoreFontData.FontFace_Default->FontFaceData  = FFontFaceData::MakeFontFaceData(CopyTemp(RawFontData));
        RawFontData.Empty(RawFontData.Num());

        if (this->LoadFontFile(FONT_FILE_NAME_BOLD, RawFontData) == false)
        {
            LOG_FATAL(LogFontSubsystem, "Failed to load raw bold core font data.")
            return;
        }
        this->CoreFontData.FontFace_Bold = NewObject<UFontFace>();
        this->CoreFontData.FontFace_Bold->LoadingPolicy = EFontLoadingPolicy::Inline;
        this->CoreFontData.FontFace_Bold->FontFaceData  = FFontFaceData::MakeFontFaceData(CopyTemp(RawFontData));
        RawFontData.Empty(RawFontData.Num());

        if (this->LoadFontFile(FONT_FILE_NAME_ITALIC, RawFontData) == false)
        {
            LOG_FATAL(LogFontSubsystem, "Failed to load raw italic core font data.")
            return;
        }
        this->CoreFontData.FontFace_Italic = NewObject<UFontFace>();
        this->CoreFontData.FontFace_Italic->LoadingPolicy = EFontLoadingPolicy::Inline;
        this->CoreFontData.FontFace_Italic->FontFaceData  = FFontFaceData::MakeFontFaceData(CopyTemp(RawFontData));
        RawFontData.Empty(RawFontData.Num());

        if (this->LoadFontFile(FONT_FILE_NAME_BOLD_ITALIC, RawFontData) == false)
        {
            LOG_FATAL(LogFontSubsystem, "Failed to load raw bold italic core font data.")
            return;
        }
        this->CoreFontData.FontFace_BoldItalic = NewObject<UFontFace>();
        this->CoreFontData.FontFace_BoldItalic->LoadingPolicy = EFontLoadingPolicy::Inline;
        this->CoreFontData.FontFace_BoldItalic->FontFaceData  = FFontFaceData::MakeFontFaceData(CopyTemp(RawFontData));
    }

    //////////////////////////////////////////////////////////////////////////
    // Supplement the core font data with a font object
    {
        this->CoreFontData.Font = NewObject<UFont>();
        this->CoreFontData.Font->FontCacheType = EFontCacheType::Runtime;

        FTypefaceEntry& TypefaceEntry_Default = this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts[this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
        TypefaceEntry_Default.Name = UFontSubsystem::GetTypeByScheme(EJAFGFontTypeFace::Default);
        TypefaceEntry_Default.Font = FFontData(this->CoreFontData.FontFace_Default);

        FTypefaceEntry& TypefaceEntry_Bold = this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts[this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
        TypefaceEntry_Bold.Name = UFontSubsystem::GetTypeByScheme(EJAFGFontTypeFace::Bold);
        TypefaceEntry_Bold.Font = FFontData(this->CoreFontData.FontFace_Bold);

        FTypefaceEntry& TypefaceEntry_Italic = this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts[this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
        TypefaceEntry_Italic.Name = UFontSubsystem::GetTypeByScheme(EJAFGFontTypeFace::Italic);
        TypefaceEntry_Italic.Font = FFontData(this->CoreFontData.FontFace_Italic);

        FTypefaceEntry& TypefaceEntry_BoldItalic = this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts[this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
        TypefaceEntry_BoldItalic.Name = UFontSubsystem::GetTypeByScheme(EJAFGFontTypeFace::BoldItalic);
        TypefaceEntry_BoldItalic.Font = FFontData(this->CoreFontData.FontFace_BoldItalic);
    }

    return;
}

void UFontSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

FName UFontSubsystem::GetTypeByScheme(const EJAFGFontTypeFace::Type InScheme)
{
    switch (InScheme)
    {
    case EJAFGFontTypeFace::Default:
    {
        return FName(TEXT("Default"));
    }
    case EJAFGFontTypeFace::Bold:
    {
        return FName(TEXT("Bold"));
    }
    case EJAFGFontTypeFace::Italic:
    {
        return FName(TEXT("Italic"));
    }
    case EJAFGFontTypeFace::BoldItalic:
    {
        return FName(TEXT("BoldItalic"));
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Could not find type face scheme: %d.", InScheme)
        return NAME_None;
    }
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UFontSubsystem::LoadFontFile(const FString& FileName, TArray<uint8>& OutBytes, TArray<FString> AllowedExtensions) const
{
    FString Unused;

    FString Extension = TEXT("");
    for (const FString& AllowedExtension : AllowedExtensions)
    {
        if (PathFinder::CreatePathToFile(nullptr, EPathType::Fonts, FString::Printf(TEXT("%s%s"), *FileName, *AllowedExtension), Unused))
        {
            Extension = AllowedExtension;
            break;
        }

        continue;
    }

    if (Extension.IsEmpty())
    {
        return false;
    }

    return PathFinder::LoadBytesFromDisk(nullptr, EPathType::Fonts, FString::Printf(TEXT("%s%s"), *FileName, *Extension), OutBytes);
}

#undef FONT_FILE_NAME_DEFAULT
#undef FONT_FILE_NAME_BOLD
#undef FONT_FILE_NAME_ITALIC
#undef FONT_FILE_NAME_BOLD_ITALIC
