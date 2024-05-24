// Copyright 2024 mzoesch. All rights reserved.

#include "TabBar/JAFGTabBarDevPanel.h"

#if UE_BUILD_SHIPPING
    #include "JAFGLogDefs.h"
#endif /* UE_BUILD_SHIPPING */

UJAFGTabBarDevPanel::UJAFGTabBarDevPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGTabBarDevPanel::NativeConstruct(void)
{
    Super::NativeConstruct();

#if UE_BUILD_SHIPPING
    LOG_FATAL(LogCommonSlate, "Not allowed in shipping builds.")
#endif /* UE_BUILD_SHIPPING */

    return;
}
