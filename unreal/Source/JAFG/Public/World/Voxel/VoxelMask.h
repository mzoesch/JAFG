// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "VoxelMask.generated.h"

class UMaterialSubsystem;
class IVoxel;

namespace ETextureGroup
{

enum Type : int8;

}

namespace ENormalLookup
{

enum Type : int8
{
	Default,
	Top,
	Bottom,
	Front,
	Side,
};

FORCEINLINE ENormalLookup::Type FromVector(const FVector& Normal)
{
	if (Normal == FVector::UpVector)
	{
		return ENormalLookup::Top;
	}

	if (Normal == FVector::DownVector)
	{
		return ENormalLookup::Bottom;
	}

	if (Normal == FVector::ForwardVector || Normal == FVector::BackwardVector)
	{
		return ENormalLookup::Front;
	}

	if (Normal == FVector::RightVector || Normal == FVector::LeftVector)
	{
		return ENormalLookup::Side;
	}

	return ENormalLookup::Default;
};

}

USTRUCT()
struct JAFG_API FVoxelMask
{
	GENERATED_BODY()

public:

	FVoxelMask(void) = default;
	explicit FVoxelMask(const FString& NameSpace, const FString& Name, const ETextureGroup::Type TextureGroup);

	static const FVoxelMask Null;
	static const FVoxelMask Air;

private:	

	/**
	 * Maps a normal vector to a texture group.
	 * A texture group can be opaque, full blend opaque, or flora blend opaque, transparent, etc.
	 */
	struct JAFG_API FTextureGroup 
	{
		FTextureGroup(void) = default;
		FTextureGroup(const ENormalLookup::Type Normal, const ETextureGroup::Type TextureGroup);

		ENormalLookup::Type Normal;
		ETextureGroup::Type TextureGroup;
	};

	/** Maps a normal vector to a texture index in the texture array. */
	struct JAFG_API FTextureIndex
	{
		FTextureIndex(void) = default;
		FTextureIndex(const ENormalLookup::Type Normal, const int32 TextureIndex);

		ENormalLookup::Type Normal;
		int32				TextureIndex;
	};
	
public:

	//////////////////////////////////////////////////////////////////////////
	// Data
	//////////////////////////////////////////////////////////////////////////
	
	FString NameSpace;
	FString Name;

private:

	/** The last index must always be the default group of this voxel. */
	TArray<FTextureGroup> TextureGroups;
	/** The last index must always be the default texture index of this voxel. */
	TArray<FTextureIndex> TextureIndices;

	/**
	 * Only during initialization. The last call to this method of an
	 * object must always contain the default texture index.
	 */
	FORCEINLINE void AddTextureIndex(const ENormalLookup::Type Normal, const int32 TextureIndex)
	{ this->TextureIndices.Add(FTextureIndex(Normal, TextureIndex)); }

	friend UMaterialSubsystem;
	
public:

	int32 GetTextureGroup(const FVector& Normal) const;
	int32 GetTextureIndex(const FVector& Normal) const;
};
