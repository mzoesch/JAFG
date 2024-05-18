// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGInputSubsystem.h"

void UJAFGInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogGameSettings, "Called.")

    this->Contexts.Empty();
    this->Actions.Empty();

    return;
}

void UJAFGInputSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UJAFGInputSubsystem::AddContext(const FJAFGInputContext& InContext)
{
    if (this->DoesContextExist(InContext.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Context already exists: %s. Discarding implementation.", *InContext.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Context already exists: %s.", *InContext.Name)
#endif /* !WITH_EDITOR */
        return;
    }

    UInputMappingContext* Context = NewObject<UInputMappingContext>();

    FJAFGPrivateInputContext PrivateContext;
    PrivateContext.Config  = InContext;
    PrivateContext.Context = Context;

    this->Contexts.Add(PrivateContext);

    LOG_VERBOSE(LogGameSettings, "Added context: %s.", *PrivateContext.Config.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFGInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* !WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name             = InAction.Name;
    PrivateAction.NorthDefaultKeyA = InAction.DefaultKeyA;
    PrivateAction.NorthDefaultKeyB = InAction.DefaultKeyB;
    PrivateAction.SouthDefaultKeyA = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyB = EKeys::Invalid;
    PrivateAction.WestDefaultKeyA  = EKeys::Invalid;
    PrivateAction.WestDefaultKeyB  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyA  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyB  = EKeys::Invalid;
    PrivateAction.Contexts         = InAction.Contexts;
    PrivateAction.bIs2DAction      = false;
    PrivateAction.Action           = Action;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyA);
        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyB);
        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *PrivateAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFGInputActionDual& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* !WITH_EDITOR */
        return;
    }

    if (InAction.Keys.Num() != InAction.Contexts.Num())
    {
#if WITH_EDITOR
        LOG_ERROR(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts do not match. Found (%d != %d).",
            *InAction.Name, InAction.Keys.Num(), InAction.Contexts.Num()
        )
#else /* WITH_EDITOR */
        LOG_FATAL(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts do not match. Found (%d != %d).",
            *InAction.Name, InAction.Keys.Num(), InAction.Contexts.Num()
        )
#endif /* !WITH_EDITOR */
        return;
    }

    if (InAction.Keys.Num() == 0 || InAction.Contexts.Num() > 2)
    {
#if WITH_EDITOR
        LOG_ERROR(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts are empty or exceed maximum of 2.",
            *InAction.Name
        )
#else /* WITH_EDITOR */
        LOG_FATAL(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts are empty or exceed maximum of 2.",
            *InAction.Name
        )
#endif /* !WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name             = InAction.Name;
    PrivateAction.NorthDefaultKeyA = InAction.Keys[0].KeyA;
    PrivateAction.NorthDefaultKeyB = InAction.Keys[0].KeyB;
    if (InAction.Keys.Num() == 2)
    {
        PrivateAction.SouthDefaultKeyA = InAction.Keys[1].KeyA;
        PrivateAction.SouthDefaultKeyB = InAction.Keys[1].KeyB;
    }
    else
    {
        PrivateAction.SouthDefaultKeyA = EKeys::Invalid;
        PrivateAction.SouthDefaultKeyB = EKeys::Invalid;
    }
    PrivateAction.WestDefaultKeyA  = EKeys::Invalid;
    PrivateAction.WestDefaultKeyB  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyA  = EKeys::Invalid;
    PrivateAction.EastDefaultKeyB  = EKeys::Invalid;
    PrivateAction.Contexts         = InAction.Contexts[0];
    if (InAction.Contexts.Num() == 2)
    {
        PrivateAction.Contexts.Append(InAction.Contexts[1]);
    }
    PrivateAction.bIs2DAction      = false;
    PrivateAction.Action           = Action;


    for (const FString& ContextName : InAction.Contexts[0])
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyA);
        Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyB);
        continue;
    }

    if (InAction.Contexts.Num() == 2)
    {
        for (const FString& ContextName : InAction.Contexts[1])
        {
            const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

            Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyA);
            Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyB);
            continue;
        }
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *PrivateAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFG2DInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* !WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name              = InAction.Name;
    PrivateAction.NorthDefaultKeyA  = InAction.NorthDefaultKeyA;
    PrivateAction.NorthDefaultKeyB  = InAction.NorthDefaultKeyB;
    PrivateAction.SouthDefaultKeyA  = InAction.SouthDefaultKeyA;
    PrivateAction.SouthDefaultKeyB  = InAction.SouthDefaultKeyB;
    PrivateAction.WestDefaultKeyA   = InAction.WestDefaultKeyA;
    PrivateAction.WestDefaultKeyB   = InAction.WestDefaultKeyB;
    PrivateAction.EastDefaultKeyA   = InAction.EastDefaultKeyA;
    PrivateAction.EastDefaultKeyB   = InAction.EastDefaultKeyB;
    PrivateAction.Contexts          = InAction.Contexts;
    PrivateAction.bIs2DAction       = true;
    PrivateAction.Action            = Action;
    PrivateAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        FEnhancedActionKeyMapping& MappingNorthA = Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyA);
        MappingNorthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        FEnhancedActionKeyMapping& MappingNorthB = Context->Context->MapKey(Action, PrivateAction.NorthDefaultKeyB);
        MappingNorthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());

        FEnhancedActionKeyMapping& MappingSouthA =  Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyA);
        MappingSouthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        MappingSouthA.Modifiers.Add(NewObject<UInputModifierNegate>());
        FEnhancedActionKeyMapping& MappingSouthB = Context->Context->MapKey(Action, PrivateAction.SouthDefaultKeyB);
        MappingSouthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
        MappingSouthB.Modifiers.Add(NewObject<UInputModifierNegate>());

        FEnhancedActionKeyMapping& MappingWestA = Context->Context->MapKey(Action, PrivateAction.WestDefaultKeyA);
        MappingWestA.Modifiers.Add(NewObject<UInputModifierNegate>());
        FEnhancedActionKeyMapping& MappingWestB = Context->Context->MapKey(Action, PrivateAction.WestDefaultKeyB);
        MappingWestB.Modifiers.Add(NewObject<UInputModifierNegate>());

                                                  Context->Context->MapKey(Action, PrivateAction.EastDefaultKeyA);
                                                  Context->Context->MapKey(Action, PrivateAction.EastDefaultKeyB);

        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *PrivateAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(FJAFG2DMouseInputAction& InAction)
{
    if (this->DoesActionExist(InAction.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Action already exists: %s. Discarding implementation.", *InAction.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Action already exists: %s.", *InAction.Name)
#endif /* !WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();
    FJAFGPrivateInputAction PrivateAction;
    PrivateAction.Name              = InAction.Name;
    PrivateAction.NorthDefaultKeyA  = EKeys::Invalid;
    PrivateAction.NorthDefaultKeyB  = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyA  = EKeys::Invalid;
    PrivateAction.SouthDefaultKeyB  = EKeys::Invalid;
    PrivateAction.WestDefaultKeyA   = EKeys::Invalid;
    PrivateAction.WestDefaultKeyB   = EKeys::Invalid;
    PrivateAction.EastDefaultKeyA   = EKeys::Invalid;
    PrivateAction.EastDefaultKeyB   = EKeys::Invalid;
    PrivateAction.Contexts          = InAction.Contexts;
    PrivateAction.bIs2DAction       = true;
    PrivateAction.Action            = Action;
    PrivateAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : PrivateAction.Contexts)
    {
        const FJAFGPrivateInputContext* Context = this->GetSafeContext(ContextName);

        Context->Context->MapKey(Action, EKeys::Mouse2D);

        continue;
    }

    this->Actions.Add(PrivateAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *InAction.Name)

    return;
}

UInputMappingContext* UJAFGInputSubsystem::GetContextByName(const FString& Name)
{
    for (FJAFGPrivateInputContext& Context : this->Contexts)
    {
        if (Context.Config.Name == Name)
        {
            return Context.Context;
        }

        continue;
    }

    return nullptr;
}

UInputMappingContext* UJAFGInputSubsystem::GetSafeContextByName(const FString& Name)
{
    if (UInputMappingContext* Context = this->GetContextByName(Name); Context)
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context with name [%s] was not found.", *Name)

    return nullptr;
}

TArray<FString> UJAFGInputSubsystem::GetAllActionNames(void) const
{
    TArray<FString> Names;

    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        Names.Add(Action.Name);
    }

    return Names;
}

UInputAction* UJAFGInputSubsystem::GetActionByName(const FString& Name)
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return Action.Action;
        }

        continue;
    }

    return nullptr;
}

TArray<FKey> UJAFGInputSubsystem::GetAllKeysForAction(const FString& Name) const
{
    if (this->DoesActionExist(Name) == false)
    {
        LOG_WARNING(LogGameSettings, "Action with name [%s] was not found.", *Name)
        return TArray<FKey>();
    }

    TArray<FKey> Keys;
    const FJAFGPrivateInputAction* Action = this->GetSafeAction(Name);

    if (Action->NorthDefaultKeyA != EKeys::Invalid)
    {
        Keys.Add(Action->NorthDefaultKeyA);
    }

    if (Action->NorthDefaultKeyB != EKeys::Invalid)
    {
        Keys.Add(Action->NorthDefaultKeyB);
    }

    if (Action->SouthDefaultKeyA != EKeys::Invalid)
    {
        Keys.Add(Action->SouthDefaultKeyA);
    }

    if (Action->SouthDefaultKeyB != EKeys::Invalid)
    {
        Keys.Add(Action->SouthDefaultKeyB);
    }

    if (Action->WestDefaultKeyA != EKeys::Invalid)
    {
        Keys.Add(Action->WestDefaultKeyA);
    }

    if (Action->WestDefaultKeyB != EKeys::Invalid)
    {
        Keys.Add(Action->WestDefaultKeyB);
    }

    if (Action->EastDefaultKeyA != EKeys::Invalid)
    {
        Keys.Add(Action->EastDefaultKeyA);
    }

    if (Action->EastDefaultKeyB != EKeys::Invalid)
    {
        Keys.Add(Action->EastDefaultKeyB);
    }

    return Keys;
}

bool UJAFGInputSubsystem::DoesContextExist(const FString& Name) const
{
    for (const auto& [Config, Context] : this->Contexts)
    {
        if (Config.Name == Name)
        {
            return true;
        }

        continue;
    }

    return false;
}

FJAFGPrivateInputContext* UJAFGInputSubsystem::GetContext(const FString& Name)
{
    for (FJAFGPrivateInputContext& Context : this->Contexts)
    {
        if (Context.Config.Name == Name)
        {
            return &Context;
        }

        continue;
    }

    return nullptr;
}

FJAFGPrivateInputContext* UJAFGInputSubsystem::GetSafeContext(const FString& Name)
{
    if (FJAFGPrivateInputContext* Context = this->GetContext(Name); Context)
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context with name [%s] was not found.", *Name)

    return nullptr;
}

bool UJAFGInputSubsystem::DoesActionExist(const FString& Name) const
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FJAFGPrivateInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return true;
        }

        continue;
    }

    return false;
}

const FJAFGPrivateInputAction* UJAFGInputSubsystem::GetAction(const FString& Name) const
{
    for (int i = 0; i < this->Actions.Num(); i++)
    {
        if (this->Actions[i].Name == Name)
        {
            return &this->Actions[i];
        }

        continue;
    }

    return nullptr;
}

const FJAFGPrivateInputAction* UJAFGInputSubsystem::GetSafeAction(const FString& Name) const
{
    if (const FJAFGPrivateInputAction* Action = this->GetAction(Name); Action)
    {
        return Action;
    }

    LOG_FATAL(LogGameSettings, "Action with name [%s] was not found.", *Name)

    return nullptr;
}
