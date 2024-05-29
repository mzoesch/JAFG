// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/JAFGInputSubsystem.h"

#include "JAFGLogDefs.h"
#include "JAFGSettingsLocal.h"

void UJAFGInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CastChecked<UJAFGSettingsLocal>(GEngine->GetGameUserSettings())->SetOwningInputSubsystem(this);

    return;
}

void UJAFGInputSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UJAFGInputSubsystem::AddUpperContext(const FJAFGUpperInputContext& InContext)
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

    if (this->DoesUpperContextExist(InContext.Name))
    {
        int32 Added = 0;
        for (const FString& Redirection : InContext.InputContextRedirections)
        {
            this->UpperContexts[InContext.Name].InputContextRedirections.AddUnique(Redirection);
            ++Added;
        }
        LOG_VERBOSE(LogGameSettings, "Added %d of %d redirections to existing upper context: %s.", Added, InContext.InputContextRedirections.Num(), *InContext.Name)
        return;
    }

    this->UpperContexts.Add(InContext.Name, FUpperContextValue(InContext.InputContextRedirections));
    LOG_VERBOSE(LogGameSettings, "Added upper context [%s] with %d redirections.", *InContext.Name, InContext.InputContextRedirections.Num())

    return;
}

void UJAFGInputSubsystem::AddContext(const FJAFGInputContext& InContext)
{
    if (this->DoesUpperContextExist(InContext.Name))
    {
#if WITH_EDITOR
        LOG_ERROR(LogGameSettings, "Upper context already exists: %s. Discarding implementation.", *InContext.Name)
#else /* WITH_EDITOR */
        LOG_FATAL(LogGameSettings, "Upper context already exists: %s.", *InContext.Name)
#endif /* !WITH_EDITOR */
        return;
    }

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

    FLoadedContext OutContext;
    OutContext.Name    = InContext.Name;
    OutContext.Context = Context;

    this->Contexts.Add(OutContext);

    LOG_VERBOSE(LogGameSettings, "Added context: %s.", *InContext.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(const FJAFGSingleInputAction& InAction)
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

    FLoadedInputAction OutAction = this->CreateEmptyAction(InAction.Name, InAction.Contexts);
    OutAction.Type = EJAFGInputActionType::Single;

    OutAction.NorthDefaultKeyA = InAction.Keys.KeyA;
    OutAction.NorthDefaultKeyB = InAction.Keys.KeyB;

    OutAction.Action = Action;

    if (InAction.Contexts.IsEmpty())
    {
        LOG_FATAL(LogGameSettings, "Action has no contexts: %s.", *InAction.Name)
    }

    for (const FString& ContextName : InAction.Contexts)
    {
        if (const FLoadedContext* Context = this->GetContext(ContextName); Context)
        {
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);

            continue;
        }

        for (const FString& Redirection : this->GetSafeUpperContext(ContextName)->InputContextRedirections)
        {
            const FLoadedContext* Context = this->GetSafeContext(Redirection);

            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);

            continue;
        }

        continue;
    }

    this->Actions.Add(OutAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *InAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(const FJAFGDualInputAction& InAction)
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
            "Action is invalid: %s. Keys and Contexts do not match. Found (%d != %d). Discarding implementation.",
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

    if (InAction.Keys.Num() != 2 || InAction.Contexts.Num() != 2)
    {
#if WITH_EDITOR
        LOG_ERROR(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts do not have the expected length of 2. Discarding implementation.",
            *InAction.Name
        )
#else /* WITH_EDITOR */
        LOG_FATAL(
            LogGameSettings,
            "Action is invalid: %s. Keys and Contexts do not have the expected length of 2.",
            *InAction.Name
        )
#endif /* !WITH_EDITOR */
        return;
    }

    UInputAction* Action = NewObject<UInputAction>();

    FLoadedInputAction OutAction;
    OutAction.Name = InAction.Name;
    OutAction.Type = EJAFGInputActionType::Dual;

    OutAction.NorthDefaultKeyA = InAction.Keys[0].KeyA;
    OutAction.NorthDefaultKeyB = InAction.Keys[0].KeyB;
    OutAction.SouthDefaultKeyA = InAction.Keys[1].KeyA;
    OutAction.SouthDefaultKeyB = InAction.Keys[1].KeyB;
    OutAction.WestDefaultKeyA  = EKeys::Invalid;
    OutAction.WestDefaultKeyB  = EKeys::Invalid;
    OutAction.EastDefaultKeyA  = EKeys::Invalid;
    OutAction.EastDefaultKeyB  = EKeys::Invalid;

    OutAction.ContextDivider   = InAction.Contexts[0].Num();
    OutAction.Contexts         = InAction.Contexts[0];
    check( OutAction.Contexts.Num() == OutAction.ContextDivider )
    OutAction.Contexts.Append(InAction.Contexts[1]);

    OutAction.Action = Action;

    for (const FString& ContextName : InAction.Contexts[0])
    {
        if (const FLoadedContext* Context = this->GetContext(ContextName); Context)
        {
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);

            continue;
        }

        for (const FString& Redirection : this->GetSafeUpperContext(ContextName)->InputContextRedirections)
        {
            const FLoadedContext* Context = this->GetSafeContext(Redirection);

            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);

            continue;
        }

        continue;
    }

    for (const FString& ContextName : InAction.Contexts[1])
    {
        if (const FLoadedContext* Context = this->GetContext(ContextName); Context)
        {
            Context->Context->MapKey(Action, OutAction.SouthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.SouthDefaultKeyB);

            continue;
        }

        for (const FString& Redirection : this->GetSafeUpperContext(ContextName)->InputContextRedirections)
        {
            const FLoadedContext* Context = this->GetSafeContext(Redirection);

            Context->Context->MapKey(Action, OutAction.SouthDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.SouthDefaultKeyB);

            continue;
        }

        continue;
    }

    this->Actions.Add(OutAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *InAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(const FJAFGTwoDimensionalInputAction& InAction)
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

    FLoadedInputAction OutAction;
    OutAction.Name              = InAction.Name;
    OutAction.Type              = EJAFGInputActionType::TwoDimensional;

    OutAction.NorthDefaultKeyA  = InAction.NorthKeys.KeyA;
    OutAction.NorthDefaultKeyA  = InAction.NorthKeys.KeyA;
    OutAction.NorthDefaultKeyB  = InAction.NorthKeys.KeyB;
    OutAction.SouthDefaultKeyA  = InAction.SouthKeys.KeyA;
    OutAction.SouthDefaultKeyB  = InAction.SouthKeys.KeyB;
    OutAction.WestDefaultKeyA   = InAction.WestKeys.KeyA;
    OutAction.WestDefaultKeyB   = InAction.WestKeys.KeyB;
    OutAction.EastDefaultKeyA   = InAction.EastKeys.KeyA;
    OutAction.EastDefaultKeyB   = InAction.EastKeys.KeyB;

    OutAction.ContextDivider    = -1;
    OutAction.Contexts          = InAction.Contexts;

    OutAction.Action            = Action;

    OutAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : OutAction.Contexts)
    {
        if (const FLoadedContext* Context = this->GetContext(ContextName); Context)
        {
            FEnhancedActionKeyMapping& MappingNorthA = Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            MappingNorthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            FEnhancedActionKeyMapping& MappingNorthB = Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);
            MappingNorthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());

            FEnhancedActionKeyMapping& MappingSouthA = Context->Context->MapKey(Action, OutAction.SouthDefaultKeyA);
            MappingSouthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            MappingSouthA.Modifiers.Add(NewObject<UInputModifierNegate>());
            FEnhancedActionKeyMapping& MappingSouthB = Context->Context->MapKey(Action, OutAction.SouthDefaultKeyB);
            MappingSouthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            MappingSouthB.Modifiers.Add(NewObject<UInputModifierNegate>());

            FEnhancedActionKeyMapping& MappingWestA = Context->Context->MapKey(Action, OutAction.WestDefaultKeyA);
            MappingWestA.Modifiers.Add(NewObject<UInputModifierNegate>());
            FEnhancedActionKeyMapping& MappingWestB = Context->Context->MapKey(Action, OutAction.WestDefaultKeyB);
            MappingWestB.Modifiers.Add(NewObject<UInputModifierNegate>());

                                                      Context->Context->MapKey(Action, OutAction.EastDefaultKeyA);
                                                      Context->Context->MapKey(Action, OutAction.EastDefaultKeyB);

            continue;
        }

        for (const FString& Redirection : this->GetSafeUpperContext(ContextName)->InputContextRedirections)
        {
            const FLoadedContext* Context = this->GetSafeContext(Redirection);

            FEnhancedActionKeyMapping& MappingNorthA = Context->Context->MapKey(Action, OutAction.NorthDefaultKeyA);
            MappingNorthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            FEnhancedActionKeyMapping& MappingNorthB = Context->Context->MapKey(Action, OutAction.NorthDefaultKeyB);
            MappingNorthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());

            FEnhancedActionKeyMapping& MappingSouthA = Context->Context->MapKey(Action, OutAction.SouthDefaultKeyA);
            MappingSouthA.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            MappingSouthA.Modifiers.Add(NewObject<UInputModifierNegate>());
            FEnhancedActionKeyMapping& MappingSouthB = Context->Context->MapKey(Action, OutAction.SouthDefaultKeyB);
            MappingSouthB.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
            MappingSouthB.Modifiers.Add(NewObject<UInputModifierNegate>());

            FEnhancedActionKeyMapping& MappingWestA = Context->Context->MapKey(Action, OutAction.WestDefaultKeyA);
            MappingWestA.Modifiers.Add(NewObject<UInputModifierNegate>());
            FEnhancedActionKeyMapping& MappingWestB = Context->Context->MapKey(Action, OutAction.WestDefaultKeyB);
            MappingWestB.Modifiers.Add(NewObject<UInputModifierNegate>());

            Context->Context->MapKey(Action, OutAction.EastDefaultKeyA);
            Context->Context->MapKey(Action, OutAction.EastDefaultKeyB);

            continue;
        }

        continue;
    }

    this->Actions.Add(OutAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *OutAction.Name)

    return;
}

void UJAFGInputSubsystem::AddAction(const FJAFGTwoDimensionalMouseInputAction& InAction)
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

    FLoadedInputAction OutAction = UJAFGInputSubsystem::CreateEmptyAction(InAction.Name, InAction.Contexts);
    OutAction.Type = EJAFGInputActionType::TwoDimensionalMouse;

    OutAction.Action = Action;
    OutAction.Action->ValueType = EInputActionValueType::Axis2D;

    for (const FString& ContextName : InAction.Contexts)
    {
        if (const FLoadedContext* Context = this->GetContext(ContextName); Context)
        {
            Context->Context->MapKey(Action, EKeys::Mouse2D);
            continue;
        }

        for (const FString& Redirection : this->GetSafeUpperContext(ContextName)->InputContextRedirections)
        {
            const FLoadedContext* Context = this->GetSafeContext(Redirection);
            Context->Context->MapKey(Action, EKeys::Mouse2D);
            continue;
        }

        continue;
    }

    this->Actions.Add(OutAction);

    LOG_VERBOSE(LogGameSettings, "Added action: %s.", *InAction.Name)

    return;
}

bool UJAFGInputSubsystem::DoesUpperContextExist(const FString& Name) const
{
    return this->UpperContexts.Contains(Name);
}

bool UJAFGInputSubsystem::DoesContextExist(const FString& Name) const
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FLoadedContext& Context : this->Contexts)
    {
        if (Context.Name == Name)
        {
            return true;
        }
    }

    return false;
}

bool UJAFGInputSubsystem::DoesActionExist(const FString& Name) const
{
    // ReSharper disable once CppUseStructuredBinding
    for (const FLoadedInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return true;
        }
    }

    return false;
}

UInputMappingContext* UJAFGInputSubsystem::GetContextValue(const FString& Name)
{
    if (const FLoadedContext* LoadedContext = this->GetContext(Name); LoadedContext)
    {
        return LoadedContext->Context;
    }
    return nullptr;
}

UInputMappingContext* UJAFGInputSubsystem::GetSafeContextValue(const FString& Name)
{
    if (UInputMappingContext* Context = this->GetContextValue(Name); Context)
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context does not exist: %s.", *Name)

    return nullptr;
}

TArray<FString> UJAFGInputSubsystem::GetAllActionNames(void)
{
    TArray<FString> Names;

    // ReSharper disable once CppUseStructuredBinding
    for (const FLoadedInputAction& Action : this->Actions)
    {
        Names.Add(Action.Name);
    }

    return Names;
}

UInputAction* UJAFGInputSubsystem::GetActionValue(const FString& Name)
{
    return this->GetAction(Name)->Action;
}

UInputAction* UJAFGInputSubsystem::GetSafeActionValue(const FString& Name)
{
    if (UInputAction* Action = this->GetActionValue(Name); Action)
    {
        return Action;
    }

    LOG_FATAL(LogGameSettings, "Action does not exist: %s.", *Name)

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
    const FLoadedInputAction* Action = this->GetSafeActionConst(Name);

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

FUpperContextValue* UJAFGInputSubsystem::GetUpperContext(const FString& Name)
{
    return this->UpperContexts.Find(Name);
}

FUpperContextValue* UJAFGInputSubsystem::GetSafeUpperContext(const FString& Name)
{
    if (FUpperContextValue* UpperContext = this->GetUpperContext(Name))
    {
        return UpperContext;
    }

    LOG_FATAL(LogGameSettings, "Upper context does not exist: %s.", *Name)

    return nullptr;
}

FLoadedContext* UJAFGInputSubsystem::GetContext(const FString& Name)
{
    for (FLoadedContext& Context : this->Contexts)
    {
        if (Context.Name == Name)
        {
            return &Context;
        }
    }

    return nullptr;
}

FLoadedContext* UJAFGInputSubsystem::GetSafeContext(const FString& Name)
{
    if (FLoadedContext* Context = this->GetContext(Name))
    {
        return Context;
    }

    LOG_FATAL(LogGameSettings, "Context does not exist: %s.", *Name)

    return nullptr;
}

FLoadedInputAction* UJAFGInputSubsystem::GetAction(const FString& Name)
{
    for (FLoadedInputAction& Action : this->Actions)
    {
        if (Action.Name == Name)
        {
            return &Action;
        }
    }

    return nullptr;
}

FLoadedInputAction* UJAFGInputSubsystem::GetSafeAction(const FString& Name)
{
    if (FLoadedInputAction* Action = this->GetAction(Name))
    {
        return Action;
    }

    LOG_FATAL(LogGameSettings, "Action does not exist: %s.", *Name)

    return nullptr;
}

const FLoadedInputAction* UJAFGInputSubsystem::GetActionConst(const FString& Name) const
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

const FLoadedInputAction* UJAFGInputSubsystem::GetSafeActionConst(const FString& Name) const
{
    if (const FLoadedInputAction* Action = this->GetActionConst(Name); Action)
    {
        return Action;
    }

    LOG_FATAL(LogGameSettings, "Action with name [%s] was not found.", *Name)

    return nullptr;
}

FLoadedInputAction UJAFGInputSubsystem::CreateEmptyAction(const FString& Name, const TArray<FString>& Contexts)
{
    FLoadedInputAction Action;

    Action.Name = Name;

    Action.Type = EJAFGInputActionType::Invalid;

    Action.NorthDefaultKeyA = EKeys::Invalid;
    Action.NorthDefaultKeyB = EKeys::Invalid;
    Action.SouthDefaultKeyA = EKeys::Invalid;
    Action.SouthDefaultKeyB = EKeys::Invalid;
    Action.WestDefaultKeyA  = EKeys::Invalid;
    Action.WestDefaultKeyB  = EKeys::Invalid;
    Action.EastDefaultKeyA  = EKeys::Invalid;
    Action.EastDefaultKeyB  = EKeys::Invalid;

    Action.ContextDivider = -1;
    Action.Contexts = Contexts;

    Action.Action = nullptr;

    return Action;
}
