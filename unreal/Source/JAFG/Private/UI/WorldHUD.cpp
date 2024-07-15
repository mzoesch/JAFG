// Copyright 2024 mzoesch. All rights reserved.

#include "UI/WorldHUD.h"

#include "CommonJAFGSlateDeveloperSettings.h"
#include "Player/WorldPlayerController.h"
#include "JAFGSlateSettings.h"
#include "Foundation/ContainerValueCursor.h"
#include "Foundation/Hotbar.h"
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

    // Hotbar
    //////////////////////////////////////////////////////////////////////////
    {
        if (SlateSettings->Hotbar == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Hotbar Widget Class is not set in project settings.")
            return;
        }
        OwningWorldController->OnWorldCharacterChange.AddLambda( [this] (AWorldCharacter* StrongOldCharacter, AWorldCharacter* NewCharacter)
        {
            if (this->Hotbar)
            {
                this->Hotbar->RemoveFromParent();
                this->Hotbar = nullptr;
            }

            if (NewCharacter)
            {
                this->Hotbar = CreateWidget<UHotbar>(this->GetWorld(), GetDefault<UJAFGSlateSettings>()->Hotbar);
                this->Hotbar->AddToViewport();
            }

            return;
        });
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

        this->EscapeMenuVisibilityChangedHandle = Cast<AWorldPlayerController>(this->GetOwningPlayerController())->SubscribeToEscapeMenuVisibilityChanged(
            FSlateVisibilityChangedSignature::FDelegate::CreateLambda( [this] (const bool bVisible)
            {
                if (this->Hotbar)
                {
                    this->Hotbar->SetVisibility(bVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
                }
                if (this->Crosshair)
                {
                    this->Crosshair->SetVisibility(bVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
                }
            })
        );
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

            NewCharacter->OnCursorValueChangedDelegate.AddLambda( [this] (void)
            {
                if (
                       this->GetOwningPlayerController()
                    && this->GetOwningPlayerController()->IsLocalController()
                    && this->GetOwningPawn()
                    && Cast<AWorldCharacter>(this->GetOwningPawn())->CursorValue != Accumulated::Null
                    )
                {
                    CreateWidget<UContainerValueCursor>(
                        this->GetWorld(),
                        GetDefault<UCommonJAFGSlateDeveloperSettings>()->ContainerValueCursorWidgetClass
                    )->AddToViewport();
                }
            });
        }

        return;
    });

    return;
}

void AWorldHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->EscapeMenuVisibilityChangedHandle.IsValid())
    {
        if (AWorldPlayerController* OwningController = Cast<AWorldPlayerController>(this->GetOwningPlayerController()); OwningController)
        {
            if (OwningController->UnSubscribeToEscapeMenuVisibilityChanged(this->EscapeMenuVisibilityChangedHandle) == false)
            {
                LOG_WARNING(LogCommonSlate, "Failed to unsubscribe from escape menu visibility changed event.")
            }
        }
    }

    return;
}

bool AWorldHUD::IsContainerRegistered(const FString& Identifier) const
{
    return this->ContainerClassMap.Contains(Identifier);
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

UJAFGContainer* AWorldHUD::PushContainerToViewport(const FString& Identifier)
{
    if (this->CurrentContainer)
    {
        LOG_FATAL(LogCommonSlate, "Current container is not null. Cannot push another container to viewport.")
        return nullptr;
    }

    if (this->IsContainerRegistered(Identifier) == false)
    {
        LOG_FATAL(LogCommonSlate, "Container with identifier %s is not registered in the container class map.", *Identifier)
        return nullptr;
    }

    Cast<AWorldCharacter>(this->GetOwningPawn())->OnStartedToggleContainer(FInputActionValue(), Identifier);

    return this->CurrentContainer;
}

void AWorldHUD::MarkCurrentContainerAsDirty(void)
{
    if (this->CurrentContainer != nullptr)
    {
        this->CurrentContainer->MarkAsDirty();
    }

    return;
}

void AWorldHUD::ConditionalMarkCurrentContainerAsDirty(const IContainer* TargetedContainer)
{
    if (this->CurrentContainer != nullptr)
    {
        this->CurrentContainer->ConditionalMarkAsDirty(TargetedContainer);
    }

    return;
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
    if (this->IsContainerRegistered(Identifier) == false)
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
