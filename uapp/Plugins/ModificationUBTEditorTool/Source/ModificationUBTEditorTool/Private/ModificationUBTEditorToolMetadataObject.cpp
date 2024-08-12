// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolMetadataObject.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Interfaces/IPluginManager.h"
#include "Widgets/SExternalImage.h"

FPluginDependencyDescriptorData::FPluginDependencyDescriptorData(void) : bEnabled(true)
{
    return;
}

void FPluginDependencyDescriptorData::PopulateFromDescriptor(const FPluginReferenceDescriptor& InDescriptor)
{
    this->GamePluginName = InDescriptor.Name;

    // Since InDescriptor.CachedJson might not contain the freshly written AdditionalFields,
    // we need to read them from the AdditionalFieldsToWrite.
    // We can easily do that by having the descriptor write its data to a temporary JSON object.
    const TSharedRef<FJsonObject> CachedJson = MakeShared<FJsonObject>();
    if (InDescriptor.CachedJson.IsValid())
    {
        FJsonObject::Duplicate(InDescriptor.CachedJson, CachedJson);
    }
    InDescriptor.UpdateJson(CachedJson.Get());
    CachedJson->TryGetStringField( TEXT("VersionRange"), this->VersionRange );

    return;
}

void FPluginDependencyDescriptorData::CopyIntoDescriptor(FPluginReferenceDescriptor& OutDescriptor) const
{
    OutDescriptor.Name     = this->GamePluginName;
    OutDescriptor.AdditionalFieldsToWrite.Add(TEXT("VersionRange"), MakeShared<FJsonValueString>(this->VersionRange));
    OutDescriptor.bEnabled = this->bEnabled;

    return;
}

UModificationUBTEditorToolMetadataObject::UModificationUBTEditorToolMetadataObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UModificationUBTEditorToolMetadataObject::PopulateFromDescriptor(const FPluginDescriptor& InDescriptor)
{
    /*
     * Since InDescriptor.CachedJson might not contain the freshly written AdditionalFields,
     * we need to read them from the AdditionalFieldsToWrite.
     * We can easily do that by having the descriptor write its data to a temporary JSON object.
     */
    const TSharedRef<FJsonObject> CachedJson = MakeShared<FJsonObject>();
    if (InDescriptor.CachedJson.IsValid())
    {
        FJsonObject::Duplicate(InDescriptor.CachedJson, CachedJson);
    }
    InDescriptor.UpdateJson(CachedJson.Get());

    this->FileVersion  = 3;
    this->Version      = InDescriptor.Version;
    this->VersionName  = InDescriptor.VersionName;
    CachedJson->TryGetStringField(TEXT("JAFGVersionRange"), this->JAFGVersionRange);
    this->EngineVersion = InDescriptor.EngineVersion;

    CachedJson->TryGetBoolField(TEXT("bClientOnly"), this->bRequiredOnClient);
    CachedJson->TryGetBoolField(TEXT("bServerOnly"), this->bRequiredOnServer);
    CachedJson->TryGetStringField(TEXT("RemoteVersionRange"), this->RemoteVersionRange);

    this->FriendlyName   = InDescriptor.FriendlyName;
    this->Description    = InDescriptor.Description;
    this->Category       = InDescriptor.Category;
    this->CreatedBy      = InDescriptor.CreatedBy;
    this->CreatedByURL   = InDescriptor.CreatedByURL;
    this->DocsURL        = InDescriptor.DocsURL;
    this->MarketplaceURL = InDescriptor.MarketplaceURL;
    this->SupportURL     = InDescriptor.SupportURL;

    CachedJson->TryGetBoolField(TEXT("bIsOptional"), this->bOptional);

    for (const FPluginReferenceDescriptor& PluginReference : InDescriptor.Plugins)
    {
        FPluginDependencyDescriptorData PluginDependency;
        PluginDependency.PopulateFromDescriptor(PluginReference);
        this->Dependencies.Add(PluginDependency);
    }

    return;
}

void UModificationUBTEditorToolMetadataObject::CopyIntoDescriptor(FPluginDescriptor& OutDescriptor) const
{
    /* OutDescriptor.AdditionalFieldsToWrite.Add(TEXT("FileVersion"), MakeShared<FJsonValueNumber>(this->FileVersion)); */
    OutDescriptor.Version            = this->Version;
    OutDescriptor.VersionName        = this->VersionName;
    OutDescriptor.JAFGVersionRange   = this->JAFGVersionRange;
    OutDescriptor.RemoteVersionRange = this->RemoteVersionRange;
    OutDescriptor.EngineVersion      = this->EngineVersion;

    OutDescriptor.bRequiredOnClient = this->bRequiredOnClient;
    OutDescriptor.bRequiredOnServer = this->bRequiredOnServer;
    OutDescriptor.bOptional         = this->bOptional;

    OutDescriptor.FriendlyName   = this->FriendlyName;
    OutDescriptor.Description    = this->Description;
    OutDescriptor.Category       = this->Category;
    OutDescriptor.CreatedBy      = this->CreatedBy;
    OutDescriptor.CreatedByURL   = this->CreatedByURL;
    OutDescriptor.DocsURL        = this->DocsURL;
    OutDescriptor.MarketplaceURL = this->MarketplaceURL;
    OutDescriptor.SupportURL     = this->SupportURL;

    TArray<FPluginReferenceDescriptor> RemovedMods = OutDescriptor.Plugins;
    for (const FPluginDependencyDescriptorData& Dependency : this->Dependencies)
    {
        RemovedMods.RemoveAll( [Dependency] (const FPluginReferenceDescriptor& PluginReference)
        {
            return Dependency.GamePluginName == PluginReference.Name;
        });

        if (
            FPluginReferenceDescriptor* ExistingPlugin = OutDescriptor.Plugins.FindByPredicate(
            [Dependency] (const FPluginReferenceDescriptor& PluginReference)
            {
                return Dependency.GamePluginName == PluginReference.Name;
            });
            ExistingPlugin
        )
        {
            Dependency.CopyIntoDescriptor(*ExistingPlugin);
        }
        else
        {
            FPluginReferenceDescriptor NewMod;
            Dependency.CopyIntoDescriptor(NewMod);
            OutDescriptor.Plugins.Add(NewMod);
        }
    }

    for (FPluginReferenceDescriptor RemovedMod : RemovedMods)
    {
        OutDescriptor.Plugins.RemoveAll( [RemovedMod] (const FPluginReferenceDescriptor& ModReference)
        {
            return ModReference.Name == RemovedMod.Name;
        });
    }

    return;
}

TSharedRef<IDetailCustomization> FModificationUBTEditorToolMetadataCustomization::MakeInstance(void)
{
    return MakeShareable(new FModificationUBTEditorToolMetadataCustomization());
}

void FModificationUBTEditorToolMetadataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);

    if (Objects.Num() == 1 && Objects[0].IsValid())
    {
        if (
            UModificationUBTEditorToolMetadataObject* PluginMetadata = Cast<UModificationUBTEditorToolMetadataObject>(Objects[0].Get());
            PluginMetadata != nullptr && PluginMetadata->TargetPluginIconPath.Len() > 0
        )
        {
            /* Get the current icon path. */
            FString CurrentIconPath = PluginMetadata->TargetPluginIconPath;
            if (FPaths::FileExists(CurrentIconPath) == false)
            {
                CurrentIconPath = IPluginManager::Get().FindPlugin(TEXT("ModificationUBTEditorTool"))->GetBaseDir() / TEXT("Resources") / TEXT("DefaultIcon128.png");
            }

            /* Add the customization to edit the icon row. */
            IDetailCategoryBuilder& ImageCategory = DetailBuilder.EditCategory(TEXT("Icon"));
            const FText IconDesc(NSLOCTEXT("PluginBrowser", "Plugin", "Icon"));
            ImageCategory.AddCustomRow(IconDesc)
            .NameContent()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding( FMargin( 0, 1, 0, 1 ) )
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(IconDesc)
                    .Font(DetailBuilder.GetDetailFont())
                ]
            ]
            .ValueContent()
            .MaxDesiredWidth(500.0f)
            .MinDesiredWidth(100.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(SExternalImage, CurrentIconPath, PluginMetadata->TargetPluginIconPath)
                    .FileDescription(IconDesc)
                    .MaxDisplaySize(FVector2D(128, 128))
                    /* UE wants the image to be 128x128, but that size is not enforced anywhere else. */
                    /* .RequiredSize(FIntPoint(128, 128)) */
                ]
            ];
        }
    }
}
