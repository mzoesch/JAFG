// Copyright 2024 mzoesch. All rights reserved.

#include "Container.h"
#include "ContainerCrafter.h"

TFunction<bool
(
    const int32 InLambdaIndex,
    IContainer* InLambdaTarget,
    IContainerOwner* InLambdaOwner
)> ELocalContainerChange::ToFunction
(
    const ELocalContainerChange::Type InType
)
{
    switch (InType)
    {
    case ELocalContainerChange::Primary:
    {
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            return InLambdaTarget->GetContainer(InLambdaIndex).OnPrimaryClicked(InLambdaOwner);
        };
    }
    case ELocalContainerChange::Secondary:
    {
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            return InLambdaTarget->GetContainer(InLambdaIndex).OnSecondaryClicked(InLambdaOwner);
        };
    }
    case ELocalContainerChange::GetProduct:
    {
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            check( Cast<IContainerCrafter>(InLambdaTarget) != nullptr )
            return Cast<IContainerCrafter>(InLambdaTarget)->OnGetContainerCrafterProduct(InLambdaOwner);
        };
    }
    default:
    {
        LOG_WARNING(LogContainerStuff, "No function for change type %s. Returning empty lambda.", *LexToString(InType))
        return [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            return false;
        };
    }
    }
}
