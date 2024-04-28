// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGFrontEnd.h"

#include "MenuHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SafeZone.h"
#include "Components/TextBlock.h"
#include "SubMenus/NewSessionFrontEnd.h"
#include "SubMenus/JoinSessionFrontEnd.h"
#include "SubMenus/EditorFrontEnd.h"
#include "SubMenus/OptionsFrontEnd.h"
#include "SubMenus/CreditsFrontEnd.h"

void UJAFGFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->ConstructWidgetSwitcher();
    this->ConstructBuildConfiguration();

    return;
}

void UJAFGFrontEnd::ConstructWidgetSwitcher(void) const
{
    if (this->NewSessionFrontEndClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "New Session Front End class is invalid.")
        return;
    }

    if (this->JoinSessionFrontEndClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Join Session Front End class is invalid.")
        return;
    }

    if (this->EditorFrontEndClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Editor Front End class is invalid.")
        return;
    }

    if (this->OptionsFrontEndClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Options Front End class is invalid.")
        return;
    }

    if (this->CreditsFrontEndClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Credits Front End class is invalid.")
        return;
    }

    this->WS_Menu->AddChild(this->WidgetTree->ConstructWidget<UWidgetSwitcher>());

    UNewSessionFrontEnd* NewSessionFrontEnd   = CreateWidget<UNewSessionFrontEnd>(this->GetWorld(), this->NewSessionFrontEndClass);
    UJoinSessionFrontEnd* JoinSessionFrontEnd = CreateWidget<UJoinSessionFrontEnd>(this->GetWorld(), this->JoinSessionFrontEndClass);
    UEditorFrontEnd* EditorFrontEnd           = CreateWidget<UEditorFrontEnd>(this->GetWorld(), this->EditorFrontEndClass);
    UOptionsFrontEnd* OptionsFrontEnd         = CreateWidget<UOptionsFrontEnd>(this->GetWorld(), this->OptionsFrontEndClass);
    UCreditsFrontEnd* CreditsFrontEnd         = CreateWidget<UCreditsFrontEnd>(this->GetWorld(), this->CreditsFrontEndClass);

    check( NewSessionFrontEnd )
    check( JoinSessionFrontEnd )
    check( EditorFrontEnd )
    check( OptionsFrontEnd )

    this->WS_Menu->AddChild(NewSessionFrontEnd);
    this->WS_Menu->AddChild(JoinSessionFrontEnd);
    this->WS_Menu->AddChild(EditorFrontEnd);

    this->WS_Menu->AddChild(OptionsFrontEnd);
    this->WS_Menu->AddChild(CreditsFrontEnd);

    this->OpenMenuTab(EMenuFrontEndTab::Invalid);

    return;
}

void UJAFGFrontEnd::OpenMenuTab(const EMenuFrontEndTab::Type MenuTab) const
{
    if (this->WS_Menu->GetActiveWidgetIndex() == static_cast<int32>(MenuTab))
    {
        if (MenuTab == EMenuFrontEndTab::Invalid)
        {
            return;
        }

        this->OpenMenuTab(EMenuFrontEndTab::Invalid);

        return;
    }

    if (MenuTab != EMenuFrontEndTab::Quit)
    {
        if (static_cast<int32>(MenuTab) >= this->WS_Menu->GetSlots().Num())
        {
            LOG_ERROR(LogCommonSlate, "MenuTab index = %d is out of range.", static_cast<int32>(MenuTab))
            return;
        }

        this->WS_Menu->SetActiveWidgetIndex(static_cast<int32>(MenuTab));
    }

    switch (MenuTab)
    {
    case EMenuFrontEndTab::Invalid:
    {
        break;
    }
    case EMenuFrontEndTab::NewSession:
    {
        this->OnNewSessionClicked();
        break;
    }
    case EMenuFrontEndTab::JoinSession:
    {
        this->OnJoinSessionClicked();
        break;
    }
    case EMenuFrontEndTab::OpenEditor:
    {
        break;
    }
    case EMenuFrontEndTab::Options:
    {
        break;
    }
    case EMenuFrontEndTab::Credits:
    {
        break;
    }
    case EMenuFrontEndTab::Quit:
    {
        this->OnQuitClicked();
        break;
    }
    default:
    {
        LOG_FATAL(LogCommonSlate, "Unknown Menu Tab index = %d.", static_cast<int32>(MenuTab))
        break;
    }
    }

    return;
}

void UJAFGFrontEnd::OnNewSessionClicked(void) const
{
    return;
}

void UJAFGFrontEnd::OnJoinSessionClicked(void) const
{
    return;
}

void UJAFGFrontEnd::OnQuitClicked(void) const
{
    Cast<AMenuHUD>(this->GetOwningPlayer()->GetHUD())->CreateWarningPopUp(
        TEXT("Are you sure you want to quit?"),
        TEXT("Quit Game"),
        [this] (const bool bAccepted) { this->OnQuitClickedDelegate(bAccepted); }
    );

    return;
}

void UJAFGFrontEnd::OnQuitClickedDelegate(const bool bAccepted) const
{
    if (bAccepted == false)
    {
        return;
    }

    //
    // Causes the Editor to crash.
    // We may want to look into this method later when we have a standalone game.
    //
    // FGenericPlatformMisc::RequestExit(false);
    //

    this->GetOwningPlayer()->ConsoleCommand(TEXT("quit"));

    return;
}

void UJAFGFrontEnd::ConstructBuildConfiguration(void) const
{
    const FString Version       = FApp::GetBuildVersion();
    const FString Configuration = LexToString(FApp::GetBuildConfiguration());

    this->BuildConfiguration->SetText(FText::FromString(FString::Printf(TEXT("%s (%s)"), *Version, *Configuration)));

    return;
}
