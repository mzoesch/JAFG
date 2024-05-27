// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/SettingsTabBarPanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/JAFGScrollBox.h"
#include "Components/JAFGTextBlock.h"
#include "Components/VerticalBox.h"
#include "SettingsData/GameSettingCollection.h"
#include "SettingsData/GameSettingValueScalar.h"
#include "Frontend/Editors/GameSettingListEntry_Scalar.h"

USettingsTabBarPanel::USettingsTabBarPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void USettingsTabBarPanel::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FSettingsPassData)
    {
        this->PageSetting = Data->PageSetting;
    }

    // delete &UncastedData;

    if (this->PageSetting == nullptr || !this->PageSetting->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid Page Setting received.")
        return;
    }

    this->CreateSettingsPage();

    return;
}

void USettingsTabBarPanel::OnNativeMadeVisible(void)
{
    Super::OnNativeMadeVisible();
}

void USettingsTabBarPanel::OnNativeMadeCollapsed(void)
{
    Super::OnNativeMadeCollapsed();
}

void USettingsTabBarPanel::CreateSettingsPage(void)
{
    LOG_DISPLAY(LogCommonSlate, "Creating Settings Page for %s", *this->PageSetting->GetDisplayName().ToString())

    for (const UGameSetting* Setting : this->PageSetting->GetChildSettings())
    {
        if (Setting == nullptr || !Setting->IsValidLowLevel())
        {
            LOG_FATAL(LogCommonSlate, "Invalid Setting received. Parent: %s.", *this->PageSetting->GetIdentifier())
            return;
        }

        if (Setting->IsA<UGameSettingCollection>())
        {
            this->CreateCollectionSubPage(Cast<UGameSettingCollection>(Setting));
        }
        else
        {
            LOG_FATAL(LogCommonSlate, "Received concrete setting. But was expecting collection. Faulty setting: %s.", *Setting->GetIdentifier())
        }

        continue;
    }

    return;
}

void USettingsTabBarPanel::CreateCollectionSubPage(const UGameSettingCollection* InCollection)
{
    LOG_DISPLAY(LogCommonSlate, "Creating Collection Sub Page for %s.", *InCollection->GetDisplayName().ToString())

    UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>();

    UJAFGTextBlock* CollectionHeader = WidgetTree->ConstructWidget<UJAFGTextBlock>();
    CollectionHeader->SetText(InCollection->GetDisplayName());
    VBox->AddChild(CollectionHeader);

    for (UGameSetting* Setting : InCollection->GetChildSettings())
    {
        if (Setting == nullptr || !Setting->IsValidLowLevel())
        {
            LOG_FATAL(LogCommonSlate, "Invalid Setting received. Parent: %s.", *InCollection->GetIdentifier())
            return;
        }

        this->CreateConcreteSetting(Setting, VBox);

        continue;
    }

    this->SB_SettingsContent->AddChild(VBox);

    return;
}

void USettingsTabBarPanel::CreateConcreteSetting(UGameSetting* InSetting, UPanelWidget* Parent)
{
    if (InSetting->IsA<UGameSettingCollection>())
    {
        this->CreateCollectionSubPage(Cast<UGameSettingCollection>(InSetting));
        return;
    }

    LOG_DISPLAY(LogCommonSlate, "Creating concrete setting: %s", *InSetting->GetIdentifier())

    if (InSetting->IsA(UGameSettingValueScalar::StaticClass()))
    {
        UGameSettingValueScalar* AsScalar = Cast<UGameSettingValueScalar>(InSetting);

        UGameSettingListEntry* Widget = CreateWidget<UGameSettingListEntry>(this->GetWorld(), this->GameSettingListEntry_ScalarWidgetClass);

        FGameSettingListEntryPassData_Scalar PassData;
        PassData.SettingName = InSetting->GetDisplayName();
        PassData.Scalar      = AsScalar;
        Widget->PassDataToWidget(PassData);

        Parent->AddChild(Widget);
    }

    else
    {
        UJAFGTextBlock* SettingText = WidgetTree->ConstructWidget<UJAFGTextBlock>();
        SettingText->SetText(InSetting->GetDisplayName());
        Parent->AddChild(SettingText);
    }


    return;
}
