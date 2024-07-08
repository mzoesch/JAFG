// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTypes.h"

#include "VoxelMask.generated.h"

class IContainerOwner;
class IContainer;
class UMaterialSubsystem;

struct FCustomSecondaryActionDelegateParams final
{
    IContainer*      Container;
    IContainerOwner* ContainerOwner;

    FCustomSecondaryActionDelegateParams(void) = default;
    FORCEINLINE FCustomSecondaryActionDelegateParams(IContainer* Container, IContainerOwner* ContainerOwner)
    {
        this->Container      = Container;
        this->ContainerOwner = ContainerOwner;

        return;
    }

};

DECLARE_DELEGATE_RetVal_OneParam(const bool /* bConsumed */, OnCustomSeconaryActionDelegateSignature, const FCustomSecondaryActionDelegateParams& /* Params */);

USTRUCT(NotBlueprintType)
struct JAFGEXTERNALCORE_API FVoxelMask
{
    GENERATED_BODY()

    //////////////////////////////////////////////////////////////////////////
    // Constructors
    /** Default constructor. Required by the engine. Do not use. */
    FVoxelMask(void)  = default;
    ~FVoxelMask(void) = default;

    FORCEINLINE explicit FVoxelMask(const FString& Namespace, const FString& Name)
    {
        this->Namespace = Namespace;
        this->Name      = Name;

        /* Filled later on during the game boot-up by the Material Subsystem. */
        this->TextureGroups.Empty();
        /* Filled later on during the game boot-up by the Material Subsystem. */
        this->TextureIndices.Empty();

        return;
    }

    FORCEINLINE explicit FVoxelMask(const FString& Namespace, const FString& Name, const ETextureGroup::Type TextureGroup)
    {
        this->Namespace = Namespace;
        this->Name      = Name;

        this->TextureGroups.Empty();
        this->TextureGroups.Add(FTextureGroup(ENormalLookup::Default, TextureGroup));

        /* Filled later on during the game boot-up by the Material Subsystem. */
        this->TextureIndices.Empty();

        return;
    }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    static const FVoxelMask Null;
    static const FVoxelMask Air;

    FString Namespace;
    FString Name;

    OnCustomSeconaryActionDelegateSignature OnCustomSecondaryActionDelegate;

    /**
     * Gets the texture group from a normal vector for this voxel.
     */
    FORCEINLINE ETextureGroup::Type GetTextureGroup(const FVector& Normal) const
    {
        const ENormalLookup::Type NormalLookup = ENormalLookup::FromVector(Normal);

        for (const FTextureGroup& TextureGroup : this->TextureGroups)
        {
            if (TextureGroup.Normal == NormalLookup)
            {
                return TextureGroup.TextureGroup;
            }

            continue;
        }

        /* The default texture group. See FVoxelMask#TextureGroups for more information. */
        return this->TextureGroups.Last().TextureGroup;
    }

    /**
     * Gets the texture index of the owning texture array at the given texture group based on a normal vector.
     */
    FORCEINLINE int32 GetTextureIndex(const FVector& Normal) const
    {
        const ENormalLookup::Type NormalLookup = ENormalLookup::FromVector(Normal);

        for (const FTextureIndex& TextureIndex : this->TextureIndices)
        {
            if (TextureIndex.Normal == NormalLookup)
            {
                return TextureIndex.TextureIndex;
            }

            continue;
        }

        /* The default texture group. See FVoxelMask#TextureIndices for more information. */
        return this->TextureIndices.Last().TextureIndex;
    }

private:

    /**
     * Maps a normal vector to a texture group.
     * A texture group can be opaque, full blend opaque, flora blend opaque, transparent, etc.
     */
    struct JAFGEXTERNALCORE_API FTextureGroup
    {
                    FTextureGroup(void) = default;
        FORCEINLINE FTextureGroup(const ENormalLookup::Type Normal, const ETextureGroup::Type TextureGroup)
        {
            this->Normal       = Normal;
            this->TextureGroup = TextureGroup;

            return;
        }

        ENormalLookup::Type Normal;
        ETextureGroup::Type TextureGroup;
    };

    /**
     * Maps a normal vector to a texture index in the owning texture array based on this normal.
     */
    struct JAFGEXTERNALCORE_API FTextureIndex
    {
                    FTextureIndex(void) = default;
        FORCEINLINE FTextureIndex(const ENormalLookup::Type Normal, const int32 TextureIndex)
        {
            this->Normal       = Normal;
            this->TextureIndex = TextureIndex;

            return;
        }

        ENormalLookup::Type Normal;
        int32               TextureIndex;
    };

    /*
     * We could use a TMap here for these two arrays. But as this is a variable that is heavily used in the chunk
     * generation and may be called multiple thousand times per frame during generation, we are currently using a
     * TArray. A Map might cause a huge unwanted overhead. There will never be more than six elements in this array
     * anyway ( because a voxel only has six sides in a three-dimensional space :D ). But we should to some performance
     * tests to be sure later on when we run into problems to find the best solution.
     */

    /** The last index must always be the default group of this voxel. */
    TArray<FTextureGroup> TextureGroups;
    /** The last index must always be the default texture index of this voxel. */
    TArray<FTextureIndex> TextureIndices;

    /**
     * Only during initialization. The last call to this method of an
     * object must always contain the default texture group.
     */
    FORCEINLINE void AddTextureGroup(const ENormalLookup::Type Normal, const ETextureGroup::Type TextureGroup)
    {
        this->TextureGroups.Add(FTextureGroup(Normal, TextureGroup));
    }

    /**
     * Only during initialization. Always ensures that the default texture group is the last one.
     * See FVoxelMask#TextureGroups for more information.
     */
    FORCEINLINE void AddSafeTextureGroup(const ENormalLookup::Type Normal, const ETextureGroup::Type TextureGroup)
    {
        if (Normal == ENormalLookup::Default)
        {
            this->TextureGroups.Add(FTextureGroup(Normal, TextureGroup));
            return;
        }

        if (this->TextureGroups.Num() == 0)
        {
            this->TextureGroups.Add(FTextureGroup(ENormalLookup::Default, TextureGroup));
            return;
        }

        this->TextureGroups.Insert(FTextureGroup(Normal, TextureGroup), this->TextureGroups.Num() - 1);
        return;
    }

    /**
     * Only during initialization. The last call to this method of an
     * object must always contain the default texture index.
     */
    FORCEINLINE void AddTextureIndex(const ENormalLookup::Type Normal, const int32 TextureIndex)
    {
        this->TextureIndices.Add(FTextureIndex(Normal, TextureIndex));
    }

    /**
     * Only during initialization. Always ensures that the default texture index is the last one.
     * See FVoxelMask#TextureIndices for more information.
     */
    FORCEINLINE void AddSafeTextureIndex(ENormalLookup::Type Normal, const int32 TextureIndex)
    {
        if (Normal == ENormalLookup::Default)
        {
            this->TextureIndices.Add(FTextureIndex(Normal, TextureIndex));
            return;
        }

        if (this->TextureIndices.Num() == 0)
        {
            this->TextureIndices.Add(FTextureIndex(ENormalLookup::Default, TextureIndex));
            return;
        }

        this->TextureIndices.Insert(FTextureIndex(Normal, TextureIndex), this->TextureIndices.Num() - 1);
        return;
    }

    /* Needs to add some textures at game boot-up. */
    friend UMaterialSubsystem;
};
