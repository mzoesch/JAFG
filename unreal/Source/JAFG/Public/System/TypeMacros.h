// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"

/**
 * Uniquely defines a chunk.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define FChunkKey FIntVector

/**
 * Uniquely defines a voxel in a chunk.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define FVoxelKey FIntVector

/**
 * Uniquely defines a voxel in the application.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define voxel_t uint32

/**
 * Uniquely defines a voxel in the application.
 * Use this sparingly and never store this type in a variable, only use it for loops or similar.
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define voxel_t_signed int64

/**
 * Defines the number of accumulated items that can be at most stored.
 * If we change this, we have to also change the FSlot struct in the Accumulated module (because there we check
 * for overflow and underflow errors).
 */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define accamount_t uint16
