// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

/* Shamelessly copied from Developer/SExternalImageReference.h / .cpp */

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "IExternalImagePickerModule.h"

class IPropertyHandle;

/**
 * Delegate fired before an image has been copied.
 *
 * @param  InChosenImage The path to the image that has been picked by the user.
 * @return True if the operation was successful.
 */
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnPreExternalImageCopyDelegateSignature, const FString& /* InChosenImage */)

/**
 * Delegate fired after an image has been copied.
 *
 * @param  InChosenImage The path to the image that has been picked by the user.
 * @return True if the operation was successful.
 */
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnPostExternalImageCopyDelegateSignature, const FString& /* InChosenImage */)

/**
 * This widget shows an external image preview of a per-project configurable image.
 * The engine provides a default, but each project may have its own override.
 */
class SExternalImage final : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SExternalImage)
      : _FileDescription(NSLOCTEXT("SExternalImageReference", "FileDescription", "External Image"))
      , _MaxDisplaySize(400.0f, 400.0f)
      , _RequiredSize(-1, -1)
      , _DeleteTargetWhenDefaultChosen(false)
    {
    }

        /** The description of the file, used in error messages/notifications. */
        SLATE_ARGUMENT(FText, FileDescription)

        /** How big the image should be displayed. */
        SLATE_ARGUMENT(FVector2D, MaxDisplaySize)

        /** How big the image needs to be (any size is allowed if this is omitted). */
        SLATE_ARGUMENT(FIntPoint, RequiredSize)

        /** Delegate fired before an image has been copied. */
        SLATE_ARGUMENT(FOnPreExternalImageCopyDelegateSignature, OnPreExternalImageCopyDelegate)

        /** Delegate fired after an image has been copied. */
        SLATE_ARGUMENT(FOnPostExternalImageCopyDelegateSignature, OnPostExternalImageCopyDeleagte)

        /** Delegate fired to get the path to start picking from. */
        SLATE_ARGUMENT(FOnGetPickerPath, OnGetPickerPath)

        /** A property handle to use if required. */
        SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

        /** If true, the target image will be deleted if the default is chosen. */
        SLATE_ARGUMENT(bool, DeleteTargetWhenDefaultChosen)

        /** File extensions allowed for the external image reference. */
        SLATE_ARGUMENT(TArray<FString>, FileExtensions)

        /** If true, deletes the previous reference if the file extension changes. */
        SLATE_ARGUMENT(bool, DeletePreviousTargetWhenExtensionChanges)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const FString& InBaseFilename, const FString& InOverrideFilename);

protected:

    /** Delegate handler for when an image is picked. */
    bool HandleExternalImagePicked(const FString& InChosenImage, const FString& InTargetImage);

    /** The image on the disk that we will use if the override does not exist. */
    FString BaseFilename;

    /** The image on the disk that the override image is stored as. */
    FString OverrideFilename;

    /** The description of the file in question, e.g. 'image' or 'icon', used for error reporting. */
    FText FileDescription;

    /** Delegate fired before an image has been copied. */
    FOnPreExternalImageCopyDelegateSignature OnPreExternalImageCopyDelegate;

    /** Delegate fired after an image has been copied. */
    FOnPostExternalImageCopyDelegateSignature OnPostExternalImageCopyDelegate;

    /** A property handle to use if required. */
    TSharedPtr<IPropertyHandle> PropertyHandle;

    /** If true, the target image will be deleted if the default is chosen. */
    bool bDeleteTargetWhenDefaultChosen = false;

    /** The extensions supported by this external reference. */
    TArray<FString> Extensions;

    /** If true, the previous target image will be deleted if the file extension changes. */
    bool bDeletePreviousTargetWhenExtensionChanges = false;
};
