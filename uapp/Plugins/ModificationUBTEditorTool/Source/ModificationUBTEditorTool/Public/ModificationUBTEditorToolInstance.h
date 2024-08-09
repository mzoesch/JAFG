// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FSerializedUATProcess;
class FModificationUBTEditorToolInstance;
struct FModificationUBTEditorToolPluginProfile;

namespace EModificationUBTEditorToolInstanceState
{

enum Type
{
    None,
    Running,
    Completed
};

}

namespace EModificationUBTEditorToolInstanceResult
{

enum Type
{
    None,
    Undetermined,
    Success,
    Fail,
    Cancelled
};

}

struct FModificationUBTEditorToolInstanceMessageEntry
{
    ELogVerbosity::Type Verbosity { ELogVerbosity::Display };
    FString Message;
};

DECLARE_MULTICAST_DELEGATE_OneParam(
    FOnModificationUBTEditorToolInstanceSpawnedDelegateSignature,
    TSharedPtr<FModificationUBTEditorToolInstance> /* NewInstance */
)
DECLARE_MULTICAST_DELEGATE_OneParam(
    FOnModificationUBTEditorToolInstanceProcessCompletedDelegateSignature,
    EModificationUBTEditorToolInstanceResult::Type /* Result */
)
DECLARE_MULTICAST_DELEGATE_OneParam(
    FOnModificationUBTEditorToolInstanceMessageReceivedDelegateSignature,
    const FModificationUBTEditorToolInstanceMessageEntry& /* Message */
)

class MODIFICATIONUBTEDITORTOOL_API FModificationUBTEditorToolInstance : public TSharedFromThis<FModificationUBTEditorToolInstance>
{
public:

    explicit FModificationUBTEditorToolInstance(
        const FString& InPluginName,
        const TSharedRef<FModificationUBTEditorToolPluginProfile>& InProfile
    );

    auto Start(void) -> bool;
    auto Cancel(void) const -> void;

    FORCEINLINE auto GetInstanceState(void) const -> EModificationUBTEditorToolInstanceState::Type { return this->InstanceState; }
    FORCEINLINE auto GetResult(void) const -> EModificationUBTEditorToolInstanceResult::Type { return this->Result; }
    FORCEINLINE auto GetEndTime(void) const -> FDateTime { return this->EndTime; }
    FORCEINLINE auto GetPluginName(void) const -> FString { return this->PluginName; }
    FORCEINLINE auto GetMessageList(void) const -> const TArray<FModificationUBTEditorToolInstanceMessageEntry>& { return this->MessageList; }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE static auto OnNewInstanceSpawned(void) -> FOnModificationUBTEditorToolInstanceSpawnedDelegateSignature&
    {
        return FModificationUBTEditorToolInstance::OnNewInstanceSpawnedDelegate;
    }
    FORCEINLINE auto OnProcessCompleted(void) -> FOnModificationUBTEditorToolInstanceProcessCompletedDelegateSignature&
    {
        return this->OnProcessCompletedDelegate;
    }
    FORCEINLINE auto OnMessageReceived(void) -> FOnModificationUBTEditorToolInstanceMessageReceivedDelegateSignature&
    {
        return this->OnMessageReceivedDelegate;
    }

    static auto GetRunningInstances(TArray<TSharedPtr<FModificationUBTEditorToolInstance>>& OutRunningInstances) -> void;

    static FOnModificationUBTEditorToolInstanceSpawnedDelegateSignature OnNewInstanceSpawnedDelegate;

private:

    auto OnWorkerMessageReceived(FString Message) -> void;
    auto OnWorkerCanceled(void) -> void;
    auto OnWorkerCompleted(const int32 ExitCode) -> void;

    auto OnWorkerMessageReceived_GameThread(const FString& Message) -> void;
    auto OnWorkerCancelled_GameThread(void) -> void;
    auto OnWorkerCompleted_GameThread(const bool bSuccess, const double Duration, const int32 ExitCode) -> void;

    auto HandleCancelButtonClicked(void) -> void;
    auto HandleDismissButtonClicked(void) -> void;
    auto HandleOutputLogNavigate(void) -> void;

    auto AddMessageToOutputLog(const ELogVerbosity::Type Verbosity, const FString& Message) -> void;

    auto SpawnNotification(void) -> void;
    auto DismissNotification(void) -> void;
    auto MarkNotificationAsSuccess(const double Duration) -> void;
    auto MarkNotificationAsFail(const int32 ExitCode) -> void;
    auto MarkNotificationCancelled(void) -> void;

    auto RegisterInGlobalList(void) -> void;
    auto RemoveFromGlobalList(void) -> void;

    static FCriticalSection GlobalListCriticalSection;
    static TArray<TSharedPtr<FModificationUBTEditorToolInstance>> GlobalList;

    EModificationUBTEditorToolInstanceState::Type  InstanceState = EModificationUBTEditorToolInstanceState::None;
    EModificationUBTEditorToolInstanceResult::Type Result        = EModificationUBTEditorToolInstanceResult::Undetermined;

    FDateTime EndTime;

    FString PluginName;
    TSharedRef<FModificationUBTEditorToolPluginProfile> Profile;

    TSharedPtr<SNotificationItem> NotificationItem;
    TSharedPtr<FSerializedUATProcess> UATProcess;
    FOnModificationUBTEditorToolInstanceProcessCompletedDelegateSignature OnProcessCompletedDelegate;

    TArray<FModificationUBTEditorToolInstanceMessageEntry> MessageList;
    FOnModificationUBTEditorToolInstanceMessageReceivedDelegateSignature OnMessageReceivedDelegate;
};
