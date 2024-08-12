// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/GamePluginOverviewPanelWidget.h"
#include "CommonHUD.h"
#include "GamePluginSettings.h"
#include "ModificationSupervisorSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGButton.h"
#include "Components/JAFGScrollBox.h"
#include "Components/JAFGTextBlock.h"
#include "Components/Spacer.h"
#include "Interfaces/IPluginManager.h"
#include "System/PluginValidationSubsystem.h"
#include "System/JAFGGameInstance.h"

bool IsPluginEnabled(const IPlugin& Plugin)
{
    if (GetDefault<UGamePluginSettings>()->IsPluginEnabled(Plugin.GetName()))
    {
        return true;
    }

    if (Plugin.GetType() == EPluginType::Mod)
    {
        return false;
    }

    return Plugin.IsEnabled();
}

UDetailedPluginInformation::UDetailedPluginInformation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UDetailedPluginInformation::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Button_Close->OnClicked.AddDynamic(this, &UDetailedPluginInformation::OnCloseClicked);

    return;
}

void UDetailedPluginInformation::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FDetailedPluginPassData)
    {
        this->Plugin = Data->Plugin;
    }

    jcheck( this->Plugin )

    this->TextBlock_PluginFriendlyName->SetText(FText::FromString(this->Plugin->GetFriendlyName()));
    this->TextBlock_EnabledState->SetText(FText::FromString(IsPluginEnabled(*this->Plugin) ? TEXT("Enabled") : TEXT("Disabled")));
    this->TextBlock_EnabledState->SetColorAndOpacity(IsPluginEnabled(*this->Plugin) ? FLinearColor::Green : FLinearColor::Red);
    this->TextBlock_PluginName->SetText(FText::FromString(this->Plugin->GetName()));
    this->TextBlock_PluginVersion->SetText(FText::FromString(FString::Printf(TEXT("%s [%d]"), *this->Plugin->GetDescriptor().VersionName, this->Plugin->GetDescriptor().Version)));
    this->TextBlock_RemotePluginVersion->SetText(FText::FromString(this->Plugin->GetDescriptor().RemoteVersionRange));
    this->TextBlock_PluginCategory->SetText(FText::FromString(this->Plugin->GetDescriptor().Category));
    this->TextBlock_Compatible->SetText(FText::FromString(this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>()->IsGamePluginCompatible(*this->Plugin) ? TEXT("Yes") : TEXT("No")));
    this->TextBlock_Compatible->SetColorAndOpacity(this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>()->IsGamePluginCompatible(*this->Plugin) ? FLinearColor::Green : FLinearColor::Red);
    this->TextBlock_JAFGVersion->SetText(FText::FromString(this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>()->GetCurrentGameVersion()));
    this->TextBlock_PluginVersionRange->SetText(FText::FromString(this->Plugin->GetDescriptor().JAFGVersionRange));
    this->TextBlock_PluginDescription->SetText(FText::FromString(this->Plugin->GetDescriptor().Description));

    return;
}

void UDetailedPluginInformation::OnCloseClicked(void)
{
    this->RemoveFromParent();
}

UGamePluginEntryWidget::UGamePluginEntryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGamePluginEntryWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    jcheck( this->DetailedPluginInformationWidgetClass )

    this->Button_ShowDetailedInformation->OnClicked.AddDynamic(this, &UGamePluginEntryWidget::OnDetailedInformationClicked);
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

    const UPluginValidationSubsystem* ModSubsystem = this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>();

    if (this->Border_PluginEntry)
    {
        if (ModSubsystem->SmartIsPluginEnabled(*this->Plugin))
        {
            this->Border_PluginEntry->LockColor();
            this->Border_PluginEntry->SetTemporarilyColor(FColor(0, 255, 0, 16), false);
        }
        else if (ModSubsystem->IsGamePluginIncompatible(*this->Plugin))
        {
            this->Border_PluginEntry->LockColor();
            this->Border_PluginEntry->SetTemporarilyColor(FColor(255, 0, 0, 16), false);
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

    if (ModSubsystem->SmartIsPluginEnabled(*this->Plugin) && ModSubsystem->IsGamePluginIncompatible(*this->Plugin))
    {
        this->TextBlock_EnabledIncompatible->SetVisibility(ESlateVisibility::Visible);
        if (this->Border_PluginEntry)
        {
            this->Border_PluginEntry->LockColor();
            this->Border_PluginEntry->SetTemporarilyColor(FColor(255, 0, 0, 32), false);
        }
    }
    else
    {
        this->TextBlock_EnabledIncompatible->SetVisibility(ESlateVisibility::Hidden);
    }

    this->TextBlock_PluginVersion->SetText(FText::FromString(FString::Printf(TEXT("%s"), *this->Plugin->GetDescriptor().VersionName)));

    this->TextBlock_PluginDescription->SetText(FText::FromString(this->Plugin->GetDescriptor().Description));

    this->Button_TogglePlugin->SetIsEnabled(this->ShouldTogglePluginButtonBeEnabled());

    if (this->IsGamePluginDivertedFromDefault())
    {
        this->Border_RestartInfoWrapper->SetVisibility(ESlateVisibility::Visible);
        this->TextBlock_RestartInfo->SetText(FText::FromString(
            this->WillGamePluginBeEnabled()
                ? TEXT("Plugin will be enabled after restart.") :
            this->WillGamePluginBeDisabled()
                ? TEXT("Plugin will be disabled after restart.") :
                TEXT("?")
        ));
    }
    else
    {
        this->Border_RestartInfoWrapper->SetVisibility(ESlateVisibility::Hidden);
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

bool UGamePluginEntryWidget::IsGamePlugin() const
{
    return this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>()->IsGamePlugin(*this->Plugin);
}

bool UGamePluginEntryWidget::IsGamePluginDivertedFromDefault(void) const
{
    return GetDefault<UGamePluginSettings>()->IsPluginDivertedFromDefault(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::WillGamePluginBeEnabled(void) const
{
    return GetDefault<UGamePluginSettings>()->WillPluginBeEnabled(this->Plugin->GetName());
}

bool UGamePluginEntryWidget::WillGamePluginBeDisabled(void) const
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

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UGamePluginEntryWidget::OnDetailedInformationClicked(void)
{
    UJAFGUserWidget* Widget = CreateWidget<UJAFGUserWidget>(this->GetWorld(), this->DetailedPluginInformationWidgetClass);
    Widget->PassDataToWidget(FDetailedPluginPassData(this->Plugin));
    Widget->AddToViewport();

    return;
}

UGamePluginOverviewPanelWidget::UGamePluginOverviewPanelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGamePluginOverviewPanelWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    jcheck( this->GamePluginEntryWidgetClass )

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

    const UPluginValidationSubsystem* ModSubsystem = this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>();
    PluginsToRender.Sort( [ModSubsystem] (const TSharedRef<IPlugin>& A, const TSharedRef<IPlugin>& B)
    {
        const bool bAEnabledAndIncompatible = ModSubsystem->SmartIsPluginEnabled(*A) && ModSubsystem->IsGamePluginIncompatible(*A);
        const bool bBEnabledAndIncompatible = ModSubsystem->SmartIsPluginEnabled(*B) && ModSubsystem->IsGamePluginIncompatible(*B);

        if (bAEnabledAndIncompatible && bBEnabledAndIncompatible == false)
        {
            return true;
        }

        if (bAEnabledAndIncompatible == false && bBEnabledAndIncompatible)
        {
            return false;
        }

        if (bAEnabledAndIncompatible && bBEnabledAndIncompatible)
        {
            return
                (A->GetFriendlyName().IsEmpty() ? A->GetName() : A->GetFriendlyName())
                    <
                (B->GetFriendlyName().IsEmpty() ? B->GetName() : B->GetFriendlyName());
        }

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
    const UGamePluginSettings*        GamePluginSettings = GetDefault<UGamePluginSettings>();
    const UPluginValidationSubsystem* ModSubsystem       = this->GetGameInstance()->GetSubsystem<UPluginValidationSubsystem>();

    /* Always show game plugins that are enabled and incompatible. */
    if (
           ModSubsystem->SmartIsPluginEnabled(*Plugin)
        && ModSubsystem->IsGamePluginIncompatible(*Plugin)
    )
    {
        return false;
    }

    if (ModSubsystem->IsGamePlugin(*Plugin) == false)
    {
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

    if (this->bShowCompatibleMods == false && ModSubsystem->IsGamePluginCompatible(*Plugin))
    {
        return true;
    }

    if (this->bShowIncompatibleMods == false && ModSubsystem->IsGamePluginIncompatible(*Plugin))
    {
        return true;
    }

    if (this->bShowServerMods == false && Plugin->GetDescriptor().bRequiredOnServer)
    {
        /* Do not hide mods that are server and client sided. */
        if ((this->bShowClientMods && Plugin->GetDescriptor().bRequiredOnClient) == false)
        {
            return true;
        }
    }

    if (this->bShowClientMods == false && Plugin->GetDescriptor().bRequiredOnClient)
    {
        /* Do not hide mods that are server and client sided. */
        if ((this->bShowServerMods && Plugin->GetDescriptor().bRequiredOnServer) == false)
        {
            return true;
        }
    }

    if (this->bShowTestMods == false && ModSubsystem->IsTestGamePlugin(*Plugin))
    {
        return true;
    }

    if (this->bShowEnabledMods == false && GamePluginSettings->IsPluginEnabled(Plugin->GetName()))
    {
        return true;
    }

    if (this->bShowDisabledMods == false && GamePluginSettings->IsPluginEnabled(Plugin->GetName()) == false)
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
