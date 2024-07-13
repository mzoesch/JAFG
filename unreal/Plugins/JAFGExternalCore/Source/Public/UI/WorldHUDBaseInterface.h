// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "WorldHUDBaseInterface.generated.h"

class IContainer;
class UJAFGContainer;

UINTERFACE()
class JAFGEXTERNALCORE_API UWorldHUDBaseInterface : public UInterface
{
    GENERATED_BODY()
};

class JAFGEXTERNALCORE_API IWorldHUDBaseInterface
{
    GENERATED_BODY()

public:

    virtual auto IsContainerRegistered(const FString& Identifier) const -> bool = 0;
    virtual auto RegisterContainer(const FString& Identifier, const TFunction<TSubclassOf<UJAFGContainer>(void)>& ContainerClassGetter) -> bool = 0;

    virtual auto PushContainerToViewport(const FString& Identifier) -> UJAFGContainer* = 0;
    template<class T = UJAFGContainer>
    auto PushContainerToViewport(const FString& Identifier) -> T*
    {
        return Cast<T>(this->PushContainerToViewport(Identifier));
    }

    virtual auto MarkCurrentContainerAsDirty(void) -> void = 0;
    /** Will only mark the container as dirty if the owning pointer of the current visible container is the same. */
    virtual auto ConditionalMarkCurrentContainerAsDirty(const IContainer* TargetedContainer) -> void = 0;
};
