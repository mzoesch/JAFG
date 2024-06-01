// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGUserWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/JAFGBorder.h"

UJAFGUserWidget::UJAFGUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGUserWidget::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->UpdateTreeSchemes();
    return;
}

void UJAFGUserWidget::UpdateTreeSchemes(void) const
{
    TArray<UWidget*> Children; this->WidgetTree->GetChildWidgets(this->WidgetTree->RootWidget, Children);

    for (UWidget* Child : Children)
    {
        if (IUsesJAFGColorScheme* ColorSchemeUser = Cast<IUsesJAFGColorScheme>(Child); ColorSchemeUser)
        {
            ColorSchemeUser->UpdateComponentWithTheirScheme();
        }
    }

    return;
}
