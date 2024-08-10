// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MaskFactory.h"
#include "CommonJAFGSlateDeveloperSettings.h"
#include "Components/ContainerReplicatorComponentBase.h"
#include "Foundation/Blueprints/CommonJAFGDualContainer.h"
#include "UI/WorldHUDBaseInterface.h"

FVoxelMask CreateMask_Barrel(void)
{
    const FString MaskName = TEXT("Barrel");

    FVoxelMask Mask = FVoxelMask(JAFGModNamespace, MaskName);

    Mask.OnCustomSecondaryActionDelegate.BindLambda(
    [MaskName] (const FCustomSecondaryActionDelegateParams& Params) -> const bool
    {
        if (Params.WorldHUD->IsContainerRegistered(UCommonJAFGDualContainer::Identifier) == false)
        {
            Params.WorldHUD->RegisterContainer(UCommonJAFGDualContainer::Identifier, [] (void) -> TSubclassOf<UJAFGContainer>
            {
                return GetDefault<UCommonJAFGSlateDeveloperSettings>()->DualContainerWidgetClass;
            });
        }

        UCommonJAFGDualContainer* Widget = Params.WorldHUD->PushContainerToViewport<UCommonJAFGDualContainer>(UCommonJAFGDualContainer::Identifier);

        if (Widget == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Failed to push container [%s] to viewport.", *UCommonJAFGDualContainer::Identifier)
            return false;
        }

        Params.ContainerOwner->GetContainerReplicatorComponent()->RequestContainerAsync(Params.WorldHitLocation, [Widget] (IContainer* Container)
        {
            if (Container == nullptr)
            {
                LOG_FATAL(LogCommonSlate, "Failed to request container for widget [%s].", *Widget->GetName())
                return;
            }

            Widget->SetOtherContainerData(Container);
            Widget->NowDoBuildDeferred();

            return;
        });

        Widget->SetOtherContainerDisplayName(MaskName);
        Widget->NowDoBuildDeferred();

        return true;
    });

    return Mask;
}
