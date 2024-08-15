// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/JAFGFrontEnd.h"

#include "Blueprint/WidgetTree.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGTextBlock.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"
#include "System/PluginValidationSubsystem.h"
#include "TabBar/JAFGTabBarButton.h"

UJAFGFrontEnd::UJAFGFrontEnd(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();

    const UPluginValidationSubsystem* Subsystem = this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>();
    TArray<FString> MaybeDangerousPlugins; Subsystem->HasAnyEnabledGamePluginsThatAreIncompatible(MaybeDangerousPlugins);
    for (const FString& Plugin : MaybeDangerousPlugins)
    {
        LOG_WARNING(LogModSubsystem, "The mod '%s' is incompatible with the current version of the game. Please disable it to avoid potential issues.", *Plugin);

        UJAFGBorder* Border = WidgetTree->ConstructWidget<UJAFGBorder>(UJAFGBorder::StaticClass());
        FSlateBrushOutlineSettings OutlineSettings;
        OutlineSettings.Color        = FLinearColor::Red;
        OutlineSettings.Width        = 2.0f;
        OutlineSettings.RoundingType = ESlateBrushRoundingType::Type::FixedRadius;
        FSlateBrush Brush;
        Brush.TintColor       = FLinearColor::Red;
        Brush.OutlineSettings = OutlineSettings;
        Brush.DrawAs          = ESlateBrushDrawType::Type::RoundedBox;
        Border->SetBrushColor(FColor(255, 0, 0, 64));
        Border->SetBrush(Brush);

        UJAFGTextBlock* TextBlock = WidgetTree->ConstructWidget<UJAFGTextBlock>(UJAFGTextBlock::StaticClass());
        TextBlock->SetColorScheme(EJAFGFontSize::Body);
        TextBlock->SetColorAndOpacity(FLinearColor(0.3f, 0.0f, 0.0f, 1.0f));
        TextBlock->SetAutoWrapText(true);
        TextBlock->SetText(FText::FromString(FString::Printf(TEXT("The mod '%s' is incompatible with the current version of the game. Please disable it to avoid potential issues."), *Plugin)));
        Border->AddChild(TextBlock);

        this->VerticalBox_ErrorMessages->AddChild(Border);

        USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
        Spacer->SetSize(FVector2D(1.0f, 10.0f));
        this->VerticalBox_ErrorMessages->AddChild(Spacer);
    }

    return;
}

void UJAFGFrontEnd::RegisterAllTabs(void)
{
    jcheck( this->HostSessionPanelWidgetClass && "Join Session Widget Class is not set." )
    FTabBarTabDescriptor Host = UJAFGTabBar::GetDefaultTabDescriptor();
    Host.Identifier       = "Host";
    Host.DisplayName      = "Host Session";
    Host.PanelWidgetClass = this->HostSessionPanelWidgetClass;

    jcheck( this->JoinSessionPanelWidgetClass && "Join Session Panel Widget Class is not set." )
    FTabBarTabDescriptor Join = UJAFGTabBar::GetDefaultTabDescriptor();
    Join.Identifier       = "Join";
    Join.DisplayName      = "Join Session";
    Join.PanelWidgetClass = this->JoinSessionPanelWidgetClass;

    jcheck( this->EncyclopediaPanelWidgetClass && "Encyclopedia Panel Widget Class is not set." )
    FTabBarTabDescriptor Encyclopedia = UJAFGTabBar::GetDefaultTabDescriptor();
    Encyclopedia.Identifier       = "Encyclopedia";
    Encyclopedia.DisplayName      = "Encyclopedia";
    Encyclopedia.PanelWidgetClass = this->EncyclopediaPanelWidgetClass;
    Encyclopedia.Padding          = FMargin(0.0f, 150.0f, 0.0f, 0.0f);

    jcheck( this->SettingsPanelWidgetClass && "Settings Panel Widget Class is not set." )
    FTabBarTabDescriptor Settings = UJAFGTabBar::GetDefaultTabDescriptor();
    Settings.Identifier       = "Settings";
    Settings.DisplayName      = "Settings";
    Settings.PanelWidgetClass = this->SettingsPanelWidgetClass;

    jcheck( this->GamePluginOverviewPanelWidgetClass && "Game Plugin Overview Panel Widget Class is not set." )
    FTabBarTabDescriptor GamePluginOverview = UJAFGTabBar::GetDefaultTabDescriptor();
    GamePluginOverview.Identifier       = "GamePluginOverview";
    GamePluginOverview.DisplayName      = "Game Plugins";
    GamePluginOverview.PanelWidgetClass = this->GamePluginOverviewPanelWidgetClass;

    jcheck( this->QuitGameButtonWidgetClass && "Quit Game Widget Class is not set." )
    FTabBarTabDescriptor Quit = UJAFGTabBar::GetDefaultTabDescriptor();
    Quit.Identifier        = "Quit";
    Quit.DisplayName       = "Quit Game";
    Quit.ButtonWidgetClass = this->QuitGameButtonWidgetClass;
    Quit.Padding           = FMargin(0.0f, 50.0f, 0.0f, 0.0f);

    this->RegisterConcreteTab(Host);
    this->RegisterConcreteTab(Join);
    this->RegisterConcreteTab(Encyclopedia);
    this->RegisterConcreteTab(Settings);
    this->RegisterConcreteTab(GamePluginOverview);
    this->RegisterConcreteTab(Quit);

    return;
}
