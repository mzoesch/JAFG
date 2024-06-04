// Copyright 2024 mzoesch. All rights reserved.

#include "UI/WorldHUD.h"

#include "Player/WorldPlayerController.h"
#include "JAFGSlateSettings.h"
#include "WorldCore/WorldCharacter.h"

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldHUD::BeginPlay(void)
{
    Super::BeginPlay();

    AWorldPlayerController* OwningWorldController = Cast<AWorldPlayerController>(this->GetOwningPlayerController());

    if (OwningWorldController == nullptr)
    {
#if WITH_EDITOR
        if (GEditor && GEditor->IsSimulateInEditorInProgress())
        {
            LOG_DISPLAY(LogCommonSlate, "Simulation mode detected. Creating simulation HUD.")
            this->CreateSimulationHUD();
            return;
        }
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController. Discarding HUD setup.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
        return;
    }

    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();

    // Crosshair
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->CrosshairWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Crosshair Widget Class is not set in project settings.")
            return;
        }
        this->Crosshair = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->CrosshairWidgetClass);
        this->Crosshair->AddToViewport();
    }

    // Quick Session Preview
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->QuickSessionPreviewWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Quick Session Preview Widget Class is not set in project settings.")
            return;
        }
        this->QuickSessionPreview = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->QuickSessionPreviewWidgetClass);
        this->QuickSessionPreview->AddToViewport();
        this->QuickSessionPreview->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Chat Menu
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->ChatMenuWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Chat Menu Widget Class is not set in project settings.")
            return;
        }
        this->ChatMenu = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->ChatMenuWidgetClass);
        this->ChatMenu->AddToViewport();
        this->ChatMenu->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Debug Screen
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->DebugScreenWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Debug Screen Widget Class is not set in project settings.")
            return;
        }
        this->DebugScreen = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->DebugScreenWidgetClass);
        this->DebugScreen->AddToViewport();
        this->DebugScreen->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Escape menu
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->EscapeMenuWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Escape Menu Widget Class is not set in project settings.")
            return;
        }
        this->EscapeMenu = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->EscapeMenuWidgetClass);
        this->EscapeMenu->AddToViewport();
        this->EscapeMenu->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Loading Screen
    //////////////////////////////////////////////////////////////////////////
    {
        this->CreateLoadingScreen();
    }

    // Container Screen
    //////////////////////////////////////////////////////////////////////////
    OwningWorldController->OnWorldCharacterChange.AddLambda( [this] (AWorldCharacter* StrongOldCharacter, AWorldCharacter* NewCharacter)
    {
        if (StrongOldCharacter && this->ContainerVisibleDelegateHandle.IsValid())
        {
            if (StrongOldCharacter->UnSubscribeToContainerVisibleEvent(this->ContainerVisibleDelegateHandle) == false)
            {
                LOG_WARNING(LogCommonSlate, "Failed to unsubscribe from container visible event. Faulty character: %s.", *StrongOldCharacter->GetName())
            }
            this->ContainerVisibleDelegateHandle.Reset();
        }
        if (StrongOldCharacter && this->ContainerLostVisibilityDelegateHandle.IsValid())
        {
            if (StrongOldCharacter->UnSubscribeToContainerLostVisibilityEvent(this->ContainerLostVisibilityDelegateHandle) == false)
            {
                LOG_WARNING(LogCommonSlate, "Failed to unsubscribe from container lost visibility event. Faulty character: %s.", *StrongOldCharacter->GetName())
            }
            this->ContainerLostVisibilityDelegateHandle.Reset();
        }

        if (NewCharacter)
        {
            this->ContainerVisibleDelegateHandle =
                NewCharacter->SubscribeToContainerVisibleEvent(FOnContainerVisibleSignature::FDelegate::CreateLambda(
                    [this] (const FString& Identifier) { this->OnContainerVisible(Identifier); }
                )); jcheck( this->ContainerVisibleDelegateHandle.IsValid() )
            this->ContainerLostVisibilityDelegateHandle =
                NewCharacter->SubscribeToContainerLostVisibilityEvent(FOnContainerLostVisibilitySignature::FDelegate::CreateLambda(
                    [this] (void) { this->OnContainerLostVisibility(); }
                )); jcheck( this->ContainerLostVisibilityDelegateHandle.IsValid() )
        }

        return;
    });

    return;
}

bool AWorldHUD::RegisterContainer(const FString& Identifier, const TFunction<TSubclassOf<UJAFGContainer>()>& ContainerClassGetter)
{
    if (this->ContainerClassMap.Contains(Identifier))
    {
        LOG_WARNING(LogCommonSlate, "Container with identifier %s is already registered.", *Identifier)
        return false;
    }

    this->ContainerClassMap.Add(Identifier, ContainerClassGetter);

    LOG_VERBOSE(LogCommonSlate, "Container with identifier %s is now registered.", *Identifier)

    return true;
}

#if WITH_EDITOR
void AWorldHUD::CreateSimulationHUD(void)
{
    // ReSharper disable once CppTooWideScope
    const UJAFGSlateSettings* SlateSettings = GetDefault<UJAFGSlateSettings>();

    // Debug Screen
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->DebugScreenWidgetClass == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Debug Screen Widget Class is not set in project settings.")
            return;
        }
        this->DebugScreen = CreateWidget<UJAFGUserWidget>(this->GetWorld(), SlateSettings->DebugScreenWidgetClass);
        this->DebugScreen->AddToViewport();
        this->DebugScreen->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}
#endif /* WITH_EDITOR */

void AWorldHUD::OnContainerVisible(const FString& Identifier)
{
    if (this->ContainerClassMap.Contains(Identifier) == false)
    {
        LOG_FATAL(LogCommonSlate, "Container with identifier %s is not registered in the container class map.", *Identifier)
        return;
    }

    this->CurrentContainer = CreateWidget<UJAFGContainer>(this->GetWorld(), this->ContainerClassMap[Identifier]());
    jcheck( this->CurrentContainer )
    this->CurrentContainer->AddToViewport();

    Cast<ACommonPlayerController>(this->GetOwningPlayerController())->ShowMouseCursor(true);

    LOG_VERBOSE(LogCommonSlate, "Container with identifier %s is now visible.", *Identifier)

    return;
}

void AWorldHUD::OnContainerLostVisibility(void)
{
    LOG_VERBOSE(LogCommonSlate, "Current container lost visibility.")

    this->CurrentContainer->RemoveFromParent();
    this->CurrentContainer = nullptr;

    Cast<ACommonPlayerController>(this->GetOwningPlayerController())->ShowMouseCursor(false);

    return;
}
