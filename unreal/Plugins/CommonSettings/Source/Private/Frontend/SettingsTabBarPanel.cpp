// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/SettingsTabBarPanel.h"

#include "CustomSettingsLocalPlayer.h"
#include "JAFGSettingsLocal.h"
#include "Blueprint/WidgetTree.h"
#include "Components/JAFGButton.h"
#include "Components/JAFGScrollBox.h"
#include "Components/JAFGTextBlock.h"
#include "Components/VerticalBox.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingValueScalar.h"
#include "Frontend/Editors/GameSettingListEntry_Color.h"
#include "Frontend/Editors/GameSettingListEntry_Scalar.h"
#include "Frontend/Editors/GameSettingListEntry_Keyin.h"
#include "SettingsData/GameSettingValueColor.h"
#include "SettingsData/GameSettingValueKeyIn.h"

USettingsTabBarPanel::USettingsTabBarPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void USettingsTabBarPanel::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->B_Apply->OnClicked.AddDynamic(this, &USettingsTabBarPanel::OnApplyClicked);
    this->B_Cancel->OnClicked.AddDynamic(this, &USettingsTabBarPanel::OnCancelClicked);

    this->B_Apply->SetIsEnabled(false);

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

void USettingsTabBarPanel::OnApplyableSettingChanged(void)
{
    this->bHasSettingChanged = true;
    this->UpdateApplyButtonState();

    return;
}

void USettingsTabBarPanel::DisallowApply(const FString& ChildIdentifier)
{
    this->ChildrenThatDisallowApply.Add(ChildIdentifier);
    this->UpdateApplyButtonState();

    return;
}

void USettingsTabBarPanel::ReleaseDisallowApply(const FString& ChildIdentifier)
{
    this->ChildrenThatDisallowApply.Remove(ChildIdentifier);
    this->UpdateApplyButtonState();

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

    UVerticalBox* VBox = this->WidgetTree->ConstructWidget<UVerticalBox>();

    UJAFGTextBlock* CollectionHeader = this->WidgetTree->ConstructWidget<UJAFGTextBlock>();
    CollectionHeader->SetText(InCollection->GetDisplayName());
    VBox->AddChild(CollectionHeader);

    for (UGameSetting* Setting : InCollection->GetChildSettings())
    {
        if (Setting == nullptr || !Setting->IsValidLowLevel())
        {
            LOG_FATAL(LogCommonSlate, "Invalid Setting received. Parent: %s.", *InCollection->GetIdentifier())
            return;
        }

        if (Setting->IsA<ULazyGameSettingCollection>())
        {
            Cast<ULazyGameSettingCollection>(Setting)->LazyInitialize(
                Cast<UCustomSettingsLocalPlayer>(GEngine->GetGamePlayer(this->GetWorld(), 0))
            );
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
        PassData.OwningPanel = this;
        PassData.Scalar      = AsScalar;
        Widget->PassDataToWidget(PassData);

        Parent->AddChild(Widget);
    }

    else if (InSetting->IsA(UGameSettingValueKeyIn::StaticClass()))
    {
        UGameSettingValueKeyIn* AsKeyIn = Cast<UGameSettingValueKeyIn>(InSetting);

        UGameSettingListEntry* Widget = CreateWidget<UGameSettingListEntry>(this->GetWorld(), this->GameSettingListEntry_KeyInWidgetClass);

        FGameSettingListEntryPassData_KeyIn PassData;
        PassData.SettingName = InSetting->GetDisplayName();
        PassData.OwningPanel = this;
        PassData.KeyIn       = AsKeyIn;
        Widget->PassDataToWidget(PassData);

        Parent->AddChild(Widget);
    }

    else if (InSetting->IsA(UGameSettingValueColor::StaticClass()))
    {
        UGameSettingValueColor* AsColor = Cast<UGameSettingValueColor>(InSetting);

        UGameSettingListEntry* Widget = CreateWidget<UGameSettingListEntry>(this->GetWorld(), this->GameSettingListEntry_ColorWidgetClass);

        FGameSettingListEntryPassData_Color PassData;
        PassData.SettingName = InSetting->GetDisplayName();
        PassData.OwningPanel = this;
        PassData.Color       = AsColor;
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

void USettingsTabBarPanel::OnApplyClicked(void)
{
    LOG_DISPLAY(LogGameSettings, "Applying settings.")

    this->B_Apply->SetIsEnabled(false);

    if (this->PageSetting->GetChildSettings().IsEmpty())
    {
        LOG_ERROR(LogCommonSlate, "No settings to apply.")
        return;
    }

    UJAFGSettingsLocal* SettingsLocal = UJAFGSettingsLocal::Get();

    if (SettingsLocal == nullptr || !SettingsLocal->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid local settings.")
        return;
    }

    SettingsLocal->ApplySettings(false);

    return;
}

void USettingsTabBarPanel::OnCancelClicked(void)
{
}

void USettingsTabBarPanel::UpdateApplyButtonState(void) const
{
    this->B_Apply->SetIsEnabled(this->bHasSettingChanged && this->ChildrenThatDisallowApply.IsEmpty());
}
