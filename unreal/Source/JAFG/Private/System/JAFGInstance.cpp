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
    
    return;
}

void UJAFGInstance::Shutdown(void)
{
    Super::Shutdown();

    return;
}
