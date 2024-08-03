// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolEditPluginDialogWidget.h"
#include "ISourceControlModule.h"
#include "ISourceControlState.h"
#include "ModificationUBTEditorToolMetadataObject.h"
#include "Interfaces/IPluginManager.h"
#include "ISourceControlProvider.h"
#include "SourceControlOperations.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorToolEditPluginDialog"

void SModificationUBTEditorToolEditPluginDialogWidget::Construct(const FArguments& InArgs, TSharedRef<IPlugin> InPlugin)
{
    this->Plugin = InPlugin;

    this->MetadataObject = NewObject<UModificationUBTEditorToolMetadataObject>();
    this->MetadataObject->TargetPluginIconPath = this->Plugin->GetBaseDir() / TEXT("Resources/Icon128.png");
    this->MetadataObject->PopulateFromDescriptor(this->Plugin->GetDescriptor());
    this->MetadataObject->AddToRoot();

    FPropertyEditorModule& EditorModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    EditorModule.RegisterCustomClassLayout(
        UModificationUBTEditorToolMetadataObject::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FModificationUBTEditorToolMetadataCustomization::MakeInstance)
    );

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ActorsUseNameArea;
    DetailsViewArgs.bHideSelectionTip = true;

    const TSharedRef<IDetailsView> PropertyView = EditorModule.CreateDetailView(DetailsViewArgs);
    PropertyView->SetObject(this->MetadataObject, true);
    PropertyView->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& InPropertyChangedEvent)
    {
        return;
    });

    SWindow::Construct(
        SWindow::FArguments()
        .ClientSize(FVector2D(800.0f, 700.0f))
        .Title(FText::Format(
            LOCTEXT("ModMetadata","{0} ({1}) Properties"),
            FText::FromString(this->Plugin->GetFriendlyName()),
            FText::FromString(this->Plugin->GetName())
        ))
        .Content()
        [
            SNew(SBorder)
            .Padding(FMargin(8.0f, 8.0f))
            [
                SNew(SVerticalBox)
                // + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Fill)
                // [
                //     SNew(SBox)
                //     .Visibility_Lambda( [this]
                //     {
                //         if (Is in game version range)
                //         {
                //             return EVisibility::Collapsed;
                //         }
                //
                //         return EVisibility::Visible;
                //     })
                //     .Content()
                //     [
                //         SNew(SButton)
                //         .ButtonColorAndOpacity(FLinearColor::Red)
                //         .Text_Lambda( [this]
                //         {
                //             return FText::Format(
                //                 LOCTEXT("UpdateGameVersionButtonLabel", "Click to update Plugin's Game Version range to {0}"),
                //                 FText::FromString(FormatGameVersionRange(TargetGameVersionRange)));
                //         })
                //         .HAlign(HAlign_Center)
                //         .OnClicked_Lambda( [this]
                //         {
                //             this->UpdatePluginToCurrentlyUsedGameVersion();
                //             return FReply::Handled();
                //         })
                //     ]
                // ]
                + SVerticalBox::Slot()
                .Padding(5)
                [
                    PropertyView
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                .HAlign(HAlign_Right)
                [
                    SNew(SButton)
                    .ContentPadding(FMargin(20.0f, 2.0f))
                    .Text(LOCTEXT("OkButtonLabel", "Ok"))
                    .OnClicked(this, &SModificationUBTEditorToolEditPluginDialogWidget::OnOkClicked)
                ]
            ]
        ]
    );

    return;
}

void SModificationUBTEditorToolEditPluginDialogWidget::UpdatePluginToCurrentlyUsedGameVersion(void)
{
    return;
}

FReply SModificationUBTEditorToolEditPluginDialogWidget::OnOkClicked(void)
{
    FPluginDescriptor OldDescriptor = this->Plugin->GetDescriptor();

    FPluginDescriptor NewDescriptor = OldDescriptor;
    this->MetadataObject->CopyIntoDescriptor(NewDescriptor);
    this->MetadataObject->RemoveFromRoot();

    this->RequestDestroyWindow();

    FString OldText; OldDescriptor.Write(OldText);
    FString NewText; NewDescriptor.Write(NewText);

    if (OldText.Compare(NewText, ESearchCase::CaseSensitive) == 0)
    {
        return FReply::Handled();
    }

    const FString DescriptorFileName = this->Plugin->GetDescriptorFileName();

    /* First attempt to check out the file if SCC is enabled. */
    if (
        const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
        SourceControlModule.IsEnabled()
    )
    {
        ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();
        if (
            const TSharedPtr<ISourceControlState, ESPMode::ThreadSafe> SourceControlState =
                SourceControlProvider.GetState(DescriptorFileName, EStateCacheUsage::ForceUpdate);
            SourceControlState.IsValid() && SourceControlState->CanCheckout()
        )
        {
            SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), DescriptorFileName);
        }
    }

    /* Write to the file and update the in-memory metadata. */
    if (FText FailReason; this->Plugin->UpdateDescriptor(NewDescriptor, FailReason) == false)
    {
        FMessageDialog::Open(EAppMsgType::Ok, FailReason);
    }

    return FReply::Handled();
}
