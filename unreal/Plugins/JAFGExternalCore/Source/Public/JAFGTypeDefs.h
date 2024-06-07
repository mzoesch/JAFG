// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Uniquely defines a coordinate in the j coordinate system.
 */
typedef FIntVector FJCoordinate;

/**
 * Uniquely defines a vertical coordinate in the j coordinate system.
 */
typedef FIntVector2 FJCoordinate2;

/**
 * Uniquely defines a chunk.
 */
typedef FIntVector FChunkKey;

/**
 * Uniquely defines a vertical chunk block.
 */
typedef FIntVector FChunkBlockKey;

/**
 * Uniquely defines a vertical chunk.
 */
typedef FIntVector2 FChunkKey2;

/**
 * Uniquely defines a voxel in a chunk.
 */
typedef FIntVector FVoxelKey;

/**
 * Uniquely defines a voxel in the application.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef uint32 voxel_t;

/**
 * Uniquely defines a voxel in the application.
 * Use this sparingly and never store this type in a variable, only use it for loops or similar.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef int64 voxel_t_signed;

/**
 * Defines the number of accumulated items that can be at most stored.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef uint16 accamount_t;

/**
 * Defines the number of accumulated items that can be at most stored with the ability to be negative.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
typedef int32 accamount_t_signed;
