// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "FrontEnd/JAFGWidget.h"

#include "OptionsFrontEnd.generated.h"

JAFG_VOID

/**
 * DO NOT USE. We should make an extra module for this. As
 * we want to properly use this widget not only in the menu.
 */
UCLASS(Abstract, Blueprintable)
class MENU_API UOptionsFrontEnd : public UJAFGWidget
{
    GENERATED_BODY()
};
