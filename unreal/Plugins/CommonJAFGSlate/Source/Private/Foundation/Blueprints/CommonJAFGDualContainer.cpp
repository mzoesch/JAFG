// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/Blueprints/CommonJAFGDualContainer.h"

#include "Container.h"
#include "Components/JAFGTextBlock.h"

UCommonJAFGDualContainer::UCommonJAFGDualContainer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bDeferredBuild     = true;
    this->OtherContainerData = nullptr;

    return;
}

void UCommonJAFGDualContainer::NativeConstruct(void)
{
    Super::NativeConstruct();
}

void UCommonJAFGDualContainer::OnBuild(void)
{
    Super::OnBuild();

    this->BuildOtherContainer();

    return;
}

void UCommonJAFGDualContainer::TryUpdateDisplayNames(void)
{
    Super::TryUpdateDisplayNames();

    if (this->TextBlock_OtherContainerDisplayName)
    {
        this->TextBlock_OtherContainerDisplayName->SetText(FText::FromString(this->GetOtherContainerDisplayName()));
    }

    return;
}

void UCommonJAFGDualContainer::BuildOtherContainer(void)
{
    this->BuildContainerWithCommonLogic(this->OtherContainerData, this->ScrollBox_OtherContainerWrapper, this->TileView_OtherContainer);
}
