// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/Blueprints/CommonJAFGDualContainer.h"

#include "Container.h"
#include "Components/ContainerReplicatorComponentBase.h"
#include "Components/JAFGTextBlock.h"

UCommonJAFGDualContainer::UCommonJAFGDualContainer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->bDeferredBuild     = true;
    this->OtherContainerData = nullptr;

    return;
}

void UCommonJAFGDualContainer::SetOtherContainerData(IContainer* Container)
{
    if (Container == nullptr)
    {
        jcheckNoEntry()
        return;
    }

    this->OtherContainerData = Container;

    this->OnOtherContainerChangedDelegateHandle = Container->OnContainerChangedDelegate.AddLambda( [this] (const ELocalContainerChange::Type InReason, const int32 InIndex)
    {
        this->MarkAsDirty();
    });

    return;
}

void UCommonJAFGDualContainer::NativeConstruct(void)
{
    Super::NativeConstruct();
}

void UCommonJAFGDualContainer::NativeDestruct(void)
{
    Super::NativeDestruct();

    if (UNetStatics::IsSafeClient(this) == false)
    {
        return;
    }

    if (this->OtherContainerData)
    {
        this->OtherContainerData->OnContainerChangedDelegate.Remove(this->OnOtherContainerChangedDelegateHandle);
    }

    if (this->bAutoUnsubscribeOtherContainerOnKill)
    {
        this->GetOwningPlayer()->FindComponentByClass<UContainerReplicatorComponentBase>()->UnsubscribeContainer(this->OtherContainerData);
    }

    return;
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
    this->BuildContainerWithCommonLogic(this->OtherContainerData, this->Panel_OtherContainerWrapper, this->TileView_OtherContainer);
}
