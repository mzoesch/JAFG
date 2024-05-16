// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputMappingContext.h"

#include "JAFGInputSubsystem.generated.h"

JAFG_VOID

class UInputAction;
class UInputMappingContext;

struct FJAFGInputContext
{
    FString Name;
};

struct FMappableKeyPair
{
    FKey KeyA;
    FKey KeyB;
};

struct FJAFGInputAction
{
    FString         Name;
    FKey            DefaultKeyA;
    FKey            DefaultKeyB;
    TArray<FString> Contexts;
};

struct FJAFGInputActionDual
{
    FString                  Name;
    TArray<FMappableKeyPair> Keys;
    TArray<TArray<FString>>  Contexts;
};

struct FJAFG2DInputAction
{
    FString         Name;
    FKey            NorthDefaultKeyA;
    FKey            NorthDefaultKeyB;
    FKey            SouthDefaultKeyA;
    FKey            SouthDefaultKeyB;
    FKey            WestDefaultKeyA;
    FKey            WestDefaultKeyB;
    FKey            EastDefaultKeyA;
    FKey            EastDefaultKeyB;
    TArray<FString> Contexts;
};

struct FJAFG2DMouseInputAction
{
    FString         Name;
    TArray<FString> Contexts;
};

#pragma region Private And Internal

//////////////////////////////////////////////////////////////////////////
// Private and internal structs for the subsystem. Do not use. Use above.
//////////////////////////////////////////////////////////////////////////

USTRUCT(NotBlueprintType)
struct FJAFGPrivateInputContext
{
    GENERATED_BODY()

    FJAFGInputContext                Config;
    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TObjectPtr<UInputMappingContext> Context;
};

USTRUCT(NotBlueprintType)
struct FJAFGPrivateInputAction
{
    GENERATED_BODY()

    /* Values copied from the public intput action structs. */
    FString                  Name;
    FKey                     NorthDefaultKeyA;
    FKey                     NorthDefaultKeyB;
    FKey                     SouthDefaultKeyA;
    FKey                     SouthDefaultKeyB;
    FKey                     WestDefaultKeyA;
    FKey                     WestDefaultKeyB;
    FKey                     EastDefaultKeyA;
    FKey                     EastDefaultKeyB;
    TArray<FString>          Contexts;

    bool bIs2DAction;

    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TObjectPtr<UInputAction> Action;
};

#pragma endregion Private And Internal

/**
 * The backbone of the input system. Handles all input related tasks.
 * Bindings, actions, key mappings / re-mappings, etc.
 *
 * Other subsystems may add their own actions and contexts to this system as it is empty by default.
 */
UCLASS(NotBlueprintable)
class JAFG_API UJAFGInputSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    auto AddContext(const FJAFGInputContext& InContext) -> void;
    auto AddAction(FJAFGInputAction& InAction) -> void;
    auto AddAction(FJAFGInputActionDual& InAction) -> void;
    auto AddAction(FJAFG2DInputAction& InAction) -> void;
    auto AddAction(FJAFG2DMouseInputAction& InAction) -> void;

    auto GetContextByName(const FString& Name) -> UInputMappingContext*;
    auto GetSafeContextByName(const FString& Name) -> UInputMappingContext*;

    auto GetAllActionNames(void) const -> TArray<FString>;
    auto GetActionByName(const FString& Name) -> UInputAction*;

private:

    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TArray<FJAFGPrivateInputContext> Contexts;
    UPROPERTY() /* Do not remove UPROPERTY - Garbage Collection!!! */
    TArray<FJAFGPrivateInputAction>  Actions;

    auto DoesContextExist(const FString& Name) const -> bool;
    auto GetContext(const FString& Name) -> FJAFGPrivateInputContext*;
    auto GetSafeContext(const FString& Name) -> FJAFGPrivateInputContext*;

    auto DoesActionExist(const FString& Name) const -> bool;
};
