// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/GamePluginOverviewPanelWidget.h"
#include "CommonHUD.h"
#include "GamePluginSettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGButton.h"
#include "Components/JAFGScrollBox.h"
#include "Components/JAFGTextBlock.h"
#include "Components/Spacer.h"
#include "Interfaces/IPluginManager.h"
#include "System/JAFGGameInstance.h"

UGamePluginEntryWidget::UGamePluginEntryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGamePluginEntryWidget::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->Button_TogglePlugin->OnClicked.AddDynamic(this, &UGamePluginEntryWidget::OnTogglePluginClicked);
    return;
}

void UGamePluginEntryWidget::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FPluginPassData)
    {
        this->Owner  = Data->Owner;
        this->Plugin = &Data->Plugin;
    }

    jcheck( this->Owner  )
    jcheck( this->Plugin )

    this->NativeRefreshWidget();

    return;
}

void UGamePluginEntryWidget::NativeRefreshWidget(void)
{
    check( this->Plugin )

    if (this->Border_PluginEntry)
    {
        if (this->IsJAFGGamePluginEnabled())
        {
            this->Border_PluginEntry->LockColor();
            this->Border_PluginEntry->SetTemporarilyColor(FColor(0, 255, 0, 16), false);
        }
        else
        {
            this->Border_PluginEntry->UnlockColor();
            this->Border_PluginEntry->ResetToColorScheme();
        }
    }

    this->TextBlock_PluginName->SetText(FText::FromString(FString::Printf(
        TEXT("%s%s"),
        *this->Plugin->GetFriendlyName(),
        this->ShowNonFriendlyName() ? *FString::Printf(TEXT(" (%s)"), *this->Plugin->GetName()) : TEXT("")
    )));

    this->TextBlock_PluginDescription->SetText(FText::FromString(this->Plugin->GetDescriptor().Description));

    this->Button_TogglePlugin->SetIsEnabled(this->ShouldTogglePluginButtonBeEnabled());

    if (this->IsJAFGGamePluginDivertedFromDefault())
    {
        this->Border_RestartInfoWrapper->SetVisibility(ESlateVisibility::Visible);
        this->TextBlock_RestartInfo->SetText(FText::FromString(
            this->WillJAFGGamePluginBeEnabled()
                ? TEXT("Plugin will be enabled after restart.") :
            this->WillJAFGGamePluginBeDisabled()
                ? TEXT("Plugin will be disabled after restart.") :
                TEXT("?")
        ));
    }
    else
    {
        this->Border_RestartInfoWrapper->SetVisibility(ESlateVisibility::Collapsed);
        this->TextBlock_RestartInfo->SetText(FText::FromString(TEXT("")));
    }

    this->RefreshWidget();

    return;
}

bool UGamePluginEntryWidget::ShowNonFriendlyName(void) const
{
    check( this->Owner )
    return this->Owner->bShowNonFriendlyName;
}

bool UGamePluginEntryWidget::IsJAFGGamePluginEnabled(void) const
{
    return
           this->Plugin->IsEnabled()
        && GetDefault<UGamePluginSettings>()->IsPluginEnabled(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::IsJAFGGamePluginDisabled(void) const
{
    return this->IsJAFGGamePluginEnabled() == false;
}

bool UGamePluginEntryWidget::IsJAFGGamePluginDivertedFromDefault(void) const
{
    return GetDefault<UGamePluginSettings>()->IsPluginDivertedFromDefault(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::WillJAFGGamePluginBeEnabled(void) const
{
    return GetDefault<UGamePluginSettings>()->WillPluginBeEnabled(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::WillJAFGGamePluginBeDisabled(void) const
{
    return GetDefault<UGamePluginSettings>()->WillPluginBeDisabled(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::IsPluginEnabled(void) const
{
    return this->Plugin->IsEnabled();
}

bool UGamePluginEntryWidget::CanPluginEverBeToggled(void) const
{
    return this->Plugin->IsEnabled() && this->Plugin->GetType() == EPluginType::Mod;
}

FString UGamePluginEntryWidget::GetReasonForPluginUnabilityForToggle(void) const
{
    if (this->CanPluginEverBeToggled())
    {
        return FString(TEXT(""));
    }

    if (this->Plugin->IsEnabled() == false)
    {
        return FString(TEXT("Plugin is disabled in the .uproject."));
    }

    if (this->Plugin->GetType() == EPluginType::Project)
    {
        return FString(TEXT("Project plugins cannot be disabled."));
    }

    if (this->Plugin->GetType() == EPluginType::Engine)
    {
        return FString(TEXT("Engine plugins cannot be disabled."));
    }

    return FString(TEXT("Cannot toggle plugin."));
}

void UGamePluginEntryWidget::OnTogglePluginClicked(void)
{
    if (this->Plugin->GetType() != EPluginType::Mod)
    {
        jrelaxedCheck(false && "Tried to toggle a plugin that is not a JAFG game plugin.")
        return;
    }

    if (this->Plugin->IsEnabled() == false)
    {
        jrelaxedCheck(false && "Tried to toggle a plugin that is disabled in the .uproject.")
        return;
    }

    UGamePluginSettings* Settings = GetMutableDefault<UGamePluginSettings>();
    Settings->TogglePluginEnabled(this->Plugin->GetName());

    this->NativeRefreshWidget();

    this->Owner->OnPluginChanged();

    return;
}

bool UGamePluginEntryWidget::ShouldTogglePluginButtonBeEnabled(void) const
{
    return
       this->Plugin->GetType() == EPluginType::Mod
    && GetDefault<UGamePluginSettings>()->GetAppliedEnabledGamePlugins().Contains(this->Plugin->GetName()) == false;
}

UGamePluginOverviewPanelWidget::UGamePluginOverviewPanelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGamePluginOverviewPanelWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Button_Apply->OnClicked.AddDynamic(this, &UGamePluginOverviewPanelWidget::OnApplyClicked);
    this->Button_ApplyAndRestart->OnClicked.AddDynamic(this, &UGamePluginOverviewPanelWidget::OnApplyAndRestartClicked);

    this->RefreshRenderedPlugins();
    this->RefreshButtonStates();

    return;
}

void UGamePluginOverviewPanelWidget::OnPluginChanged(void) const
{
    this->RefreshButtonStates();
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UGamePluginOverviewPanelWidget::OnNativePluginFilterChanged(void)
{
    this->RefreshRenderedPlugins();
    this->RefreshButtonStates();

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UGamePluginOverviewPanelWidget::NativeRefreshRenderedPlugins(void)
{
    for (UWidget* Widget : this->ScrollBox_GamePlugins->GetAllChildren())
    {
        if (UGamePluginEntryWidget* GamePluginEntryWidget = Cast<UGamePluginEntryWidget>(Widget))
        {
            GamePluginEntryWidget->NativeRefreshWidget();
        }

        continue;
    }

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UGamePluginOverviewPanelWidget::OnApplyClicked(void)
{
    this->ApplySettings();
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UGamePluginOverviewPanelWidget::OnApplyAndRestartClicked(void)
{
    this->ApplySettings();
    this->RestartGame();

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UGamePluginOverviewPanelWidget::ShouldApplyButtonsBeEnabled(void) const
{
    return GetDefault<UGamePluginSettings>()->HasPendingChanges();
}

void UGamePluginOverviewPanelWidget::ApplySettings(void) const
{
    GetMutableDefault<UGamePluginSettings>()->ApplyChangedPluginSettings();

    this->RefreshRenderedPlugins();

    return;
}

void UGamePluginOverviewPanelWidget::RestartGame(void) const
{
    if (UJAFGGameInstance* GameInstance = this->GetGameInstance<UJAFGGameInstance>(); GameInstance->DoesPlatformSupportRestart())
    {
        this->GetCommonHUD()->CreateWarningPopup(
            TEXT("The game will now restart to apply the changes."),
            TEXT("Restart Game"),
            [GameInstance] (void)
            {
                GameInstance->RequestControlledRestart();
            }
        );
    }
    else
    {
        this->GetCommonHUD()->CreateWarningPopup(
            TEXT("The game cannot be restarted on this platform. The game will quit. Please restart the game manually."),
            TEXT("Quit Game"),
            [GameInstance] (void)
            {
                GameInstance->RequestControlledRestart();
            }
        );
    }

    return;
}

void UGamePluginOverviewPanelWidget::RefreshRenderedPlugins(void) const
{
    this->ScrollBox_GamePlugins->ClearChildren();

    TArray<TSharedRef<IPlugin>> PluginsToRender;

    for (const TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetDiscoveredPlugins())
    {
        if (this->SkipPlugin(Plugin))
        {
            continue;
        }

        PluginsToRender.Add(Plugin);

        continue;
    }

    PluginsToRender.Sort( [] (const TSharedRef<IPlugin>& A, const TSharedRef<IPlugin>& B)
    {
        return
            (A->GetFriendlyName().IsEmpty() ? A->GetName() : A->GetFriendlyName())
                <
            (B->GetFriendlyName().IsEmpty() ? B->GetName() : B->GetFriendlyName());
    });

    bool bFirst = true;
    for (const TSharedRef<IPlugin> Plugin : PluginsToRender)
    {
        if (bFirst == false)
        {
            USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
            Spacer->SetSize(FVector2D(1, 8));
            this->ScrollBox_GamePlugins->AddChild(Spacer);
        }

        bFirst = false;

        UJAFGUserWidget* Widget = CreateWidget<UJAFGUserWidget>(this->GetWorld(), this->GamePluginEntryWidgetClass);
        /*
         * We will now change some colors when passing the data. But the native construct, that usually should to this,
         * will be called next tick.
         */
        Widget->UpdateTreeSchemes();
        Widget->PassDataToWidget(FPluginPassData(this, Plugin));
        this->ScrollBox_GamePlugins->AddChild(Widget);
    }

    this->RefreshButtonStates();

    return;
}

bool DoesPluginHaveRuntime(const IPlugin& Plugin)
{
    /* Content plugins always have runtime component. */
    if (Plugin.GetDescriptor().bCanContainContent)
    {
        return true;
    }

    /* C++ plugins have a runtime component as long as one of their modules does. */
    for (const FModuleDescriptor& Module : Plugin.GetDescriptor().Modules)
    {
        if (
               Module.Type == EHostType::Runtime
            || Module.Type == EHostType::RuntimeNoCommandlet
            || Module.Type == EHostType::RuntimeAndProgram
            || Module.Type == EHostType::ClientOnly
            || Module.Type == EHostType::ClientOnlyNoCommandlet
            || Module.Type == EHostType::ServerOnly
            || Module.Type == EHostType::CookedOnly
        )
        {
            return true;
        }
    }

    return false;
}

bool UGamePluginOverviewPanelWidget::SkipPlugin(const TSharedRef<IPlugin>& Plugin) const
{
    if (Plugin->GetType() != EPluginType::Mod)
    {
        if (
                this->bShowOnlyServerPlugins      || this->bShowOnlyClientPlugins       || this->bShowOnlyOptionalPlugins
            ||  this->bShowOnlyEnabledGamePlugins || this->bShowOnlyDisabledGamePlugins
        )
        {
            return true;
        }

        if (
               ( this->bShowProjectPlugins && Plugin->GetType() == EPluginType::Project )
            || ( this->bShowEnginePlugins  && Plugin->GetType() == EPluginType::Engine  )
        )
        {
            if (DoesPluginHaveRuntime(Plugin.Get()))
            {
                return false;
            }
        }

        if (this->bShowNonRuntimePlugins && DoesPluginHaveRuntime(Plugin.Get()))
        {
            return false;
        }

        return true;
    }

    if (this->bShowOnlyServerPlugins && Plugin->GetDescriptor().bServerOnly == false)
    {
        return true;
    }

    if (this->bShowOnlyClientPlugins && Plugin->GetDescriptor().bClientOnly == false)
    {
        return true;
    }

    if (this->bShowOnlyOptionalPlugins && Plugin->GetDescriptor().bOptional == false)
    {
        return true;
    }

    const UGamePluginSettings* GamePluginSettings = GetDefault<UGamePluginSettings>();

    if (
           this->bShowOnlyEnabledGamePlugins
        && (
               GamePluginSettings->GetEnabledGamePlugins().Contains(Plugin->GetName()) == false
            || GamePluginSettings->GetEnabledGamePlugins()[Plugin->GetName()] == false
        )
    )
    {
        return true;
    }

    if (
           this->bShowOnlyDisabledGamePlugins
        && (
               GamePluginSettings->GetEnabledGamePlugins().Contains(Plugin->GetName())
            || GamePluginSettings->GetEnabledGamePlugins()[Plugin->GetName()]
        )
    )
    {
        return true;
    }

    return false;
}

void UGamePluginOverviewPanelWidget::RefreshButtonStates(void) const
{
    this->Button_Apply->SetIsEnabled(this->ShouldApplyButtonsBeEnabled());
    this->Button_ApplyAndRestart->SetIsEnabled(this->ShouldApplyButtonsBeEnabled());

    return;
}
