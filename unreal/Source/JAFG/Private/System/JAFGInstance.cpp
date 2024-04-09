// Copyright 2024 mzoesch. All rights reserved.

#include "System/JAFGInstance.h"



void UJAFGInstance::Init(void)
{
    Super::Init();

    check( this->MOpaque )
    check( this->MFullBlendOpaque )
    check( this->MFloraBlendOpaque )

    /* Forward initializations */

    //// Instance initialization initializations. ////

    /* Post instance initialization initializations. */

    // ...

    LOG_VERBOSE(LogHyperlane, "Initialized instance.")

    LOG_DISPLAY(LogHyperlane, "Initialized instance.")
    LOG_WARNING(LogHyperlane, "Initialized instance.")
    LOG_ERROR(LogHyperlane, "Initialized instance.")


    return;
}

void UJAFGInstance::Shutdown(void)
{
    Super::Shutdown();

    return;
}
