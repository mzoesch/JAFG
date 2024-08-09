// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "SExternalImage.h"
#include "IExternalImagePickerModule.h"
#include "HAL/FileManager.h"
#include "SourceControlOperations.h"
#include "ISourceControlProvider.h"
#include "ISourceControlModule.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "SExternalImageReference"

void SExternalImage::Construct(const FArguments& InArgs, const FString& InBaseFilename, const FString& InOverrideFilename)
{
    this->FileDescription                           = InArgs._FileDescription;
    this->OnPreExternalImageCopyDelegate            = InArgs._OnPreExternalImageCopyDelegate;
    this->OnPostExternalImageCopyDelegate           = InArgs._OnPostExternalImageCopyDeleagte;
    this->BaseFilename                              = InBaseFilename;
    this->OverrideFilename                          = InOverrideFilename;
    this->bDeleteTargetWhenDefaultChosen            = InArgs._DeleteTargetWhenDefaultChosen;
    this->bDeletePreviousTargetWhenExtensionChanges = InArgs._DeletePreviousTargetWhenExtensionChanges;
    this->Extensions                                = InArgs._FileExtensions;

    if (
        const FString BaseFilenameExtension = FPaths::GetExtension(this->BaseFilename);
        this->Extensions.Contains(BaseFilenameExtension) == false
    )
    {
       this-> Extensions.Add(BaseFilenameExtension);
    }

    FExternalImagePickerConfiguration ImageReferenceConfig;
    ImageReferenceConfig.TargetImagePath             = InOverrideFilename;
    ImageReferenceConfig.DefaultImagePath            = InBaseFilename;
    ImageReferenceConfig.OnExternalImagePicked       = FOnExternalImagePicked::CreateSP(this, &SExternalImage::HandleExternalImagePicked);
    ImageReferenceConfig.RequiredImageDimensions     = InArgs._RequiredSize;
    ImageReferenceConfig.bRequiresSpecificSize       = InArgs._RequiredSize.X >= 0;
    ImageReferenceConfig.MaxDisplayedImageDimensions = InArgs._MaxDisplaySize;
    ImageReferenceConfig.OnGetPickerPath             = InArgs._OnGetPickerPath;
    ImageReferenceConfig.FileExtensions              = InArgs._FileExtensions;

    this->ChildSlot
    [
        IExternalImagePickerModule::Get().MakeEditorWidget(ImageReferenceConfig)
    ];

    return;
}

bool SExternalImage::HandleExternalImagePicked(const FString& InChosenImage, const FString& InTargetImage)
{
    const FString TargetImagePathNoExtension =
        FPaths::GetPath(InTargetImage) / FPaths::GetBaseFilename(InTargetImage) + TEXT(".");

    if (this->bDeleteTargetWhenDefaultChosen && InChosenImage == this->BaseFilename)
    {
        for (FString& Ex : this->Extensions)
        {
            const FString TargetImagePath = TargetImagePathNoExtension + Ex;
            IFileManager& FileManager     = IFileManager::Get();

            if (FileManager.FileExists(*TargetImagePath) == false)
            {
                continue;
            }

            // We elect to remove the target image completely if the default image is chosen
            // (thus allowing us to distinguish the default from a non-default image).
            if (ISourceControlModule::Get().IsEnabled())
            {
                ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();

                const FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(
                    TargetImagePath, EStateCacheUsage::ForceUpdate);

                if (
                    const bool bIsSourceControlled = SourceControlState.IsValid() && SourceControlState->IsSourceControlled();
                    bIsSourceControlled
                )
                {
                    /* The file is managed by source control. Delete it through there. */
                    TArray<FString> DeleteFilenames;
                    DeleteFilenames.Add(TargetImagePath);

                    /* Revert the file if it is checked out. */
                    const bool bIsAdded = SourceControlState->IsAdded();
                    if (SourceControlState->IsCheckedOut() || bIsAdded || SourceControlState->IsDeleted())
                    {
                        SourceControlProvider.Execute(ISourceControlOperation::Create<FRevert>(), DeleteFilenames);
                    }

                    /* If it wasn't already marked as an add, we can ask the source control provider to delete the file. */
                    if (!bIsAdded)
                    {
                        /* Open the file for delete. */
                        SourceControlProvider.Execute(ISourceControlOperation::Create<FDelete>(), DeleteFilenames);
                    }
                }
            }

            constexpr bool bRequireExists  = false;
            constexpr bool bEvenIfReadOnly = true;
            constexpr bool bQuiet          = true;
            FileManager.Delete(*TargetImagePath, bRequireExists, bEvenIfReadOnly, bQuiet);
        }

        return true;
    }

    if (this->OnPreExternalImageCopyDelegate.IsBound())
    {
        if (this->OnPreExternalImageCopyDelegate.Execute(InChosenImage) == false)
        {
            return false;
        }
    }

    /* New target image file extension from chosen image. */
    FString NewTargetImage;

    if (FPaths::GetExtension(InTargetImage) != FPaths::GetExtension(InChosenImage))
    {
        if (this->bDeletePreviousTargetWhenExtensionChanges)
        {
            IFileManager& FileManager = IFileManager::Get();
            constexpr bool bRequireExists  = false;
            constexpr bool bEvenIfReadOnly = true;
            constexpr bool bQuiet          = true;
            FileManager.Delete(*InTargetImage, bRequireExists, bEvenIfReadOnly, bQuiet);
        }

        NewTargetImage = TargetImagePathNoExtension + FPaths::GetExtension(InChosenImage);
    }
    else
    {
        NewTargetImage = InTargetImage;
    }

    // Here the default SExternalImage would copy the file using source control, but we don't want that
    // since plugins are .gitignored and that causes an error from the source control plugin.
    if ((IFileManager::Get().Copy(*NewTargetImage, *InChosenImage, true, true) == COPY_OK) == false)
    {
        return false;
    }

    if (this->OnPostExternalImageCopyDelegate.IsBound())
    {
        if (this->OnPostExternalImageCopyDelegate.Execute(InChosenImage) == false)
        {
            return false;
        }
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
