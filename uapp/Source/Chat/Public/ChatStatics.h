// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

namespace ChatStatics
{

static const FString AuthorityName       = TEXT("AUTHORITY");
static const FString InternalName        = TEXT("INTERNAL");

static constexpr int32 MaxChatInputLength { 0x7F };

FORCEINLINE bool IsTextToLong(const FText& Text)
{
    return Text.ToString().Len() > MaxChatInputLength;
}

FORCEINLINE bool IsTextBlank(const FText& Text)
{
    for (const TCHAR& Char : Text.ToString())
    {
        if (Char != TEXT(' '))
        {
            return false;
        }
    }

    return true;
}

FORCEINLINE bool IsTextValid(const FText& Text)
{
    return
           Text.ToString().Len()           >= 1
        && Text.ToString().IsEmpty()       == false
        && ChatStatics::IsTextBlank(Text)  == false
        && ChatStatics::IsTextToLong(Text) == false;
}

}
