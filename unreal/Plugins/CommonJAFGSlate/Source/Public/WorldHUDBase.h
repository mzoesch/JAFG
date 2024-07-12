// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonHUD.h"
#include "UI/WorldHUDBaseInterface.h"

#include "WorldHUDBase.generated.h"

class UJAFGContainer;

UCLASS(Abstract, NotBlueprintable)
class COMMONJAFGSLATE_API AWorldHUDBase : public ACommonHUD, public IWorldHUDBaseInterface
{
    GENERATED_BODY()

public:

    explicit AWorldHUDBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual auto IsContainerRegistered(const FString& Identifier) const -> bool override PURE_VIRTUAL(AWorldHUDBase::IsContainerRegistered, return false;)
    virtual auto RegisterContainer(const FString& Identifier, const TFunction<TSubclassOf<UJAFGContainer>(void)>& ContainerClassGetter) -> bool override PURE_VIRTUAL(AWorldHUDBase::RegisterContainer, return false;)

    virtual auto PushContainerToViewport(const FString& Identifier) -> UJAFGContainer* override PURE_VIRTUAL(AWorldHUDBase::PushContainerToViewport, return nullptr;)

    virtual auto MarkCurrentContainerAsDirty(void) -> void override PURE_VIRTUAL(AWorldHUDBase::MarkCurrentContainerAsDirty)
};
