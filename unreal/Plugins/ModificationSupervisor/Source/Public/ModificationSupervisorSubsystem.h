// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalModificationSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoxelMask.h"
#include "ItemMask.h"

#include "ModificationSupervisorSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInitializeOptionalVoxelsDelegateSignature, TArray<FVoxelMask>& /* VoxelMasks */)
DECLARE_MULTICAST_DELEGATE_OneParam(FInitializeOptionalItemsDelegateSignature,  TArray<FItemMask>&  /* ItemMasks  */)

UCLASS(NotBlueprintable)
class MODIFICATIONSUPERVISOR_API UModificationSupervisorSubsystem final : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    explicit UModificationSupervisorSubsystem();

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation

    static inline TArray<TSubclassOf<UExternalModificationSubsystem>> ModSubsystems;

    static void AddMod(const TSubclassOf<UExternalModificationSubsystem> ModSubsystem);

    FInitializeOptionalVoxelsDelegateSignature InitializeOptionalVoxelsDelegate;
    FInitializeOptionalItemsDelegateSignature  InitializeOptionalItemsDelegate;
};
