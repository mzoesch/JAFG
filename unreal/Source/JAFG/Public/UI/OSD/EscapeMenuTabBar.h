// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TabBar/JAFGTabBar.h"

#include "EscapeMenuTabBar.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenuTabBar : public UJAFGTabBar
{
    GENERATED_BODY()

public:

    explicit UEscapeMenuTabBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    virtual void NativeDestruct(void) override;
    // ~UUserWidget implementation

    FDelegateHandle VisibilityShouldChangeDelegateHandle;
    void OnVisibilityShouldChange(const bool bVisible);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarButton> ResumeEntryButtonWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarButton> ExitToMenuEntryButtonWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UJAFGTabBarButton> ExitToDesktopEntryButtonWidgetClass;

    virtual void RegisterAllTabs(void) override;
};
