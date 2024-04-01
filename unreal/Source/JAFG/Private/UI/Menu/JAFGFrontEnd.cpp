// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Menu/JAFGFrontEnd.h"

#include "Components/TextBlock.h"
#include "Misc/App.h"
#include "UI/Common/CommonHUD.h"
#include "UI/Common/CommonWarningPopUpWidget.h"
#include "UI/Menu/MenuJoinSessionFrontEnd.h"
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

    if (MenuTab != EMenuTab::Quit)
    {
        if (static_cast<int32>(MenuTab) >= this->WS_Menu->GetSlots().Num())
        {
            UE_LOG(LogTemp, Fatal, TEXT("UJAFGFrontEnd::OpenMenuTab: MenuTab index = %d is out of range."), static_cast<int32>(MenuTab))
            return;
        }

        this->WS_Menu->SetActiveWidgetIndex(static_cast<int32>(MenuTab));
    }

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
    case EMenuTab::OpenEditor:
        break;
    case EMenuTab::Options:
        break;
    case EMenuTab::Credits:
        break;
    case EMenuTab::Quit:
        this->OnQuitClicked();
        break;
    default:
        UE_LOG(LogTemp, Fatal, TEXT("UJAFGFrontEnd::OpenMenuTab: Unknown MenuTab index = %d."), static_cast<int32>(MenuTab))
        break;
    }

    return;
}

void UJAFGFrontEnd::OnNewSessionClicked(void) const
{
    this->WB_TabNewSession->ReloadLocalSaves();
}

void UJAFGFrontEnd::OnJoinSessionClicked() const
{
    this->WB_TabJoinSession->ReloadFoundSessions();
}

void UJAFGFrontEnd::OnQuitClicked() const
{
    FWarningPopUpWidgetData Data = FWarningPopUpWidgetData(TEXT("Are you sure you want to quit?"));
    CastChecked<ACommonHUD>(this->GetOwningPlayer()->GetHUD())->CreateWarningOptionPopUp(Data, [this](const bool bAccepted) { this->OnQuitClickedDelegate(bAccepted); });
}

void UJAFGFrontEnd::OnQuitClickedDelegate(const bool bAccepted) const
{
    if (bAccepted == false)
    {
        return;
    }

    //
    // Causes the Editor to crash. We may want to look into this method later when we have a standalone game.
    //
    // FGenericPlatformMisc::RequestExit(false);
    //

    this->GetOwningPlayer()->ConsoleCommand("quit");

    return;
}

void UJAFGFrontEnd::ConstructBuildConfiguration(void) const
{
    const FString Version       = FApp::GetBuildVersion();
    const FString Configuration = LexToString(FApp::GetBuildConfiguration());

    this->BuildConfiguration->SetText(FText::FromString(FString::Printf(TEXT("%s (%s)"), *Version, *Configuration)));

    return;
}
