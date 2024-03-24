// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/JAFGFrontEnd.h"

#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "Misc/App.h"
#include "UI/Menu/MenuNewSessionFrontEnd.h"

void UJAFGFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->ConstructBuildConfiguration();

    return;
}

void UJAFGFrontEnd::OpenMenuTab(const EMenuTab MenuTab) const
{
    if (this->WS_Menu->GetActiveWidgetIndex() == static_cast<int32>(MenuTab))
    {
        if (MenuTab == EMenuTab::Invalid)
        {
            return;
        }

        this->OpenMenuTab(EMenuTab::Invalid);
        
        return;
    }
    
    this->WS_Menu->SetActiveWidgetIndex(static_cast<int32>(MenuTab));

    switch (MenuTab)
    {
    case EMenuTab::Invalid:
        break;
    case EMenuTab::NewSession:
        this->OnNewSessionClicked();
        break;
    case EMenuTab::JoinSession:
        this->OnJoinSessionClicked();
        break;
    default:
        UE_LOG(LogTemp, Fatal, TEXT("UJAFGFrontEnd::OpenMenuTab: Unknown MenuTab index = %d."), static_cast<int32>(MenuTab))
        break;
    }

    return;
}

void UJAFGFrontEnd::OnNewSessionClicked() const
{
    this->WB_TabNewSession->ReloadLocalSaves();
}

void UJAFGFrontEnd::ConstructBuildConfiguration(void) const
{
    const FString Version       = FApp::GetBuildVersion();
    const FString Configuration = LexToString(FApp::GetBuildConfiguration());

    this->BuildConfiguration->SetText(FText::FromString(FString::Printf(TEXT("%s (%s)"), *Version, *Configuration)));

    return;
}
