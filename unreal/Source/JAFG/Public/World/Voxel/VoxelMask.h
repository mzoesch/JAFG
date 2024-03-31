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

/**
 * Has to be a USTRUCT to be used in a an TArray.
 * If this overhead causes performance issues, the struct should be converted to a c++ struct.
 * And instead of an TArray we should use a pointer. For a future problem :D.
 */
USTRUCT()
struct JAFG_API FVoxelMask
{
	GENERATED_BODY()

public:

	FVoxelMask(void) = default;
	explicit FVoxelMask(const FString& NameSpace, const FString& Name, const ETextureGroup::Type TextureGroup);
	explicit FVoxelMask(const FString& NameSpace, const FString& Name, const TMap<ENormalLookup::Type, ETextureGroup::Type>& TextureGroup);

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

	/*
	 * We could use a TMap here. But as this is a variable that is heavily used in the chunk generation and may called
	 * multiple thousand times per frame during generation, we are currently using a TArray. A Map might cause a huge
	 * unwanted overhead. There will never be more than 6 elements in this array anyway. But we should to some
	 * performance tests to be sure later on when we run into problems to find the best solution.
	 */
	
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

	/*
	 * Maybe we can to some caching here? But we should do some performance tests first.
	 */
	
	auto GetTextureGroup(const FVector& Normal) const -> ETextureGroup::Type;
	auto GetTextureIndex(const FVector& Normal) const -> int32;
};
