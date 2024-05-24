// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/HostSessionPanelWidget.h"

#include "JAFGFocusableUserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "UI/FrontEnd/LocalSaveEntry.h"

void UHostSessionPanelWidget::NativeConstruct(void)
{
    Super::NativeConstruct();
}

void UHostSessionPanelWidget::OnNativeMadeVisible(void)
{
    Super::OnNativeMadeVisible();

    check( this->SB_SavedLocalSessions )
    check( this->ET_SessionName )
    check( this->ET_NewSessionMaxPublicConnections )

    if (this->bBoundToDynamicWidgetEvents == false)
    {
        this->ET_SessionName->OnTextChanged.AddDynamic(this, &UHostSessionPanelWidget::OnSessionNameChanged);
        this->ET_NewSessionMaxPublicConnections->OnTextChanged.AddDynamic(this, &UHostSessionPanelWidget::OnNewSessionMaxPublicConnectionsChanged);
        this->bBoundToDynamicWidgetEvents = true;
    }

    return;
}

void UHostSessionPanelWidget::LoadLocalSessionsToScrollBox(void)
{
    this->SB_SavedLocalSessions->ScrollToStart();
    this->SB_SavedLocalSessions->ClearChildren();

    for (int i = 0; i < 20; ++i)
    {
        ULocalSaveEntry* LocalSaveEntry = CreateWidget<ULocalSaveEntry>(this, this->LocalSaveEntryWidgetClass);

        FPassedLocalSaveEntryData Data = FPassedLocalSaveEntryData();
        Data.WidgetIdentifier          = i;
        Data.OnWidgetPressedDelegate   = TFunction<void(int32 WidgetIdentifier)>( [this] (const int32 WidgetIdentifier)
        {
            this->OnNativeLocalSaveEntryClicked(WidgetIdentifier);
            this->OnLocalSaveEntryClicked(WidgetIdentifier);
        });
        Data.SaveName                  = FString::Printf(TEXT("Local Save Entry %d"), i);

        LocalSaveEntry->PassDataToWidget(Data);

        this->SB_SavedLocalSessions->AddChild(LocalSaveEntry);
    }

    return;
}

FString UHostSessionPanelWidget::GetSavePathForSessionName(void) const
{
    return FString::Printf(TEXT("~MyPath/%s"), *this->NewSessionName);
}

bool UHostSessionPanelWidget::IsHostFromNewSaveInputValid(void) const
{
    if (this->NewSessionName.IsEmpty() || this->NewSessionName.Len() > this->MaxSessionNameLength)
    {
        return false;
    }

    if (this->MaxPublicConnections < 0 || this->MaxPublicConnections > this->MaxPublicConnectionsLength)
    {
        return false;
    }

    return true;
}

void UHostSessionPanelWidget::OnNativeLocalSaveEntryClicked(const int32 WidgetIdentifier)
{
    if (this->HasFocusedLocalSaveEntry())
    {
        Cast<UJAFGFocusableUserWidget>(this->SB_SavedLocalSessions->GetChildAt(this->CurrentlyFocusedLocalSaveEntryIndex))->SetWidgetUnfocus();
    }

    if (this->CurrentlyFocusedLocalSaveEntryIndex == WidgetIdentifier)
    {
        this->CurrentlyFocusedLocalSaveEntryIndex = this->InvalidFocusedLocalSaveEntryIndex;
        return;
    }

    this->CurrentlyFocusedLocalSaveEntryIndex = WidgetIdentifier;
    Cast<UJAFGFocusableUserWidget>(this->SB_SavedLocalSessions->GetChildAt(this->CurrentlyFocusedLocalSaveEntryIndex))->SetWidgetFocus();

    return;
}

void UHostSessionPanelWidget::OnSessionNameChanged(const FText& Text)
{
    if (Text.ToString().Len() <= 0)
    {
        this->NewSessionName = this->DefaultSessionName;
        return;
    }

    if (Text.ToString().Len() > this->MaxSessionNameLength)
    {
        this->ET_SessionName->SetText(FText::FromString(this->NewSessionName));
        return;
    }

    this->NewSessionName = Text.ToString();

    return;
}

void UHostSessionPanelWidget::OnNewSessionMaxPublicConnectionsChanged(const FText& Text)
{
    if (Text.ToString().Len() <= 0)
    {
        this->MaxPublicConnections              = 0;
        this->LastValidMaxPublicConnectionsText = FText::GetEmpty();
        return;
    }

    if (Text.IsNumeric() == false) {
        this->ET_NewSessionMaxPublicConnections->SetText(this->LastValidMaxPublicConnectionsText);
        return;
    }

    if (FCString::Atoi(*Text.ToString()) > this->MaxPublicConnectionsLength)
    {
        this->ET_NewSessionMaxPublicConnections->SetText(this->LastValidMaxPublicConnectionsText);
        return;
    }

    this->MaxPublicConnections              = FCString::Atoi(*Text.ToString());
    this->LastValidMaxPublicConnectionsText = Text;

    return;
}

void UHostSessionPanelWidget::ResetHostFromSessionSettingsToDefault(void)
{
    this->CurrentlyFocusedLocalSaveEntryIndex = this->InvalidFocusedLocalSaveEntryIndex;
}

void UHostSessionPanelWidget::ResetHostFromNewSessionSettingsToDefault(void)
{
    this->NewSessionName = this->DefaultSessionName;
    this->LastValidMaxPublicConnectionsText = FText::FromString(TEXT("0"));
    this->MaxPublicConnections = 0;

    this->ET_SessionName->SetText(FText::FromString(this->NewSessionName));
    this->ET_NewSessionMaxPublicConnections->SetText(this->LastValidMaxPublicConnectionsText);

    return;
}

void UHostSessionPanelWidget::HostSessionFromNewSave(void) const
{
    if (this->IsHostFromNewSaveInputValid() == false)
    {
        LOG_FATAL(LogCommonSlate, "Front end provided invalid input for hosting a new session.")
        return;
    }

    ULocalSessionSupervisorSubsystem* LSSSS = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
    LSSSS->HostListenServer(this->NewSessionName, this->MaxPublicConnections, false);

    return;
}
