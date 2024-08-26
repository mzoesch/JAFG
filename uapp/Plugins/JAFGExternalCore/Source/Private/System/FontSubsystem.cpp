// Copyright 2024 mzoesch. All rights reserved.

#include "System/FontSubsystem.h"
#include "Components/RichTextBlock.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "System/PathFinder.h"

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

    TArray<uint8> RawFontData;
    if (PathFinder::LoadBytesFromDisk(nullptr, EPathType::Fonts, TEXT("Core.ttf"), RawFontData) == false)
    {
        LOG_FATAL(LogFontSubsystem, "Failed to load raw core font data.")
        return;
    }

    this->CoreFontData = FJAFGFontData();
    this->CoreFontData.FontFace = NewObject<UFontFace>();
    this->CoreFontData.FontFace->LoadingPolicy = EFontLoadingPolicy::Inline;
    this->CoreFontData.FontFace->FontFaceData  = FFontFaceData::MakeFontFaceData(CopyTemp(RawFontData));

    this->CoreFontData.Font = NewObject<UFont>();
    this->CoreFontData.Font->FontCacheType = EFontCacheType::Runtime;
    FTypefaceEntry& TypefaceEntry = this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts[this->CoreFontData.Font->CompositeFont.DefaultTypeface.Fonts.AddDefaulted()];
    TypefaceEntry.Font = FFontData(this->CoreFontData.FontFace);

    return;
}

void UFontSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}
