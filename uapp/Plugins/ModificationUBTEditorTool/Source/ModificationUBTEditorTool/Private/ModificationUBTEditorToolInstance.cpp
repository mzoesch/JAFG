// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolInstance.h"
#include "JAFGMacros.h"
#include "ModificationUBTEditorTool.h"
#include "ModificationUBTEditorToolLogWidget.h"
#include "ModificationUBTEditorToolPluginProfile.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/MonitoredProcess.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorTool"

FOnModificationUBTEditorToolInstanceSpawnedDelegateSignature FModificationUBTEditorToolInstance::OnNewInstanceSpawnedDelegate;
FCriticalSection FModificationUBTEditorToolInstance::GlobalListCriticalSection;
TArray<TSharedPtr<FModificationUBTEditorToolInstance>> FModificationUBTEditorToolInstance::GlobalList;

FModificationUBTEditorToolInstance::FModificationUBTEditorToolInstance(
    const FString& InPluginName,
    const TSharedRef<FModificationUBTEditorToolPluginProfile>& InProfile
) : PluginName(InPluginName), Profile(InProfile)
{
    return;
}

bool FModificationUBTEditorToolInstance::Start(void)
{
    jcheck( IsInGameThread() )
    jcheck(
           this->InstanceState == EModificationUBTEditorToolInstanceState::None
        || this->InstanceState == EModificationUBTEditorToolInstanceState::Completed
    )

    this->UATProcess = MakeShareable(new FSerializedUATProcess(this->Profile->MakeUATCommandLine()));

    this->UATProcess->OnOutput().BindSP(this, &FModificationUBTEditorToolInstance::OnWorkerMessageReceived);
    this->UATProcess->OnCanceled().BindSP(this, &FModificationUBTEditorToolInstance::OnWorkerCanceled);
    this->UATProcess->OnCompleted().BindSP(this, &FModificationUBTEditorToolInstance::OnWorkerCompleted);

    this->UATProcess->Launch();

    this->InstanceState = EModificationUBTEditorToolInstanceState::Running;
    this->Result        = EModificationUBTEditorToolInstanceResult::Undetermined;
    this->MessageList.Empty();

    this->SpawnNotification();

    this->RegisterInGlobalList();

    return true;
}

void FModificationUBTEditorToolInstance::Cancel(void) const
{
    jcheck( IsInGameThread() || IsInSlateThread() )
    jcheck( this->InstanceState == EModificationUBTEditorToolInstanceState::Running )

    if (this->UATProcess.IsValid())
    {
        this->UATProcess->Cancel();
    }

    return;
}

void FModificationUBTEditorToolInstance::GetRunningInstances(TArray<TSharedPtr<FModificationUBTEditorToolInstance>>& OutRunningInstances)
{
    FScopeLock Lock(&FModificationUBTEditorToolInstance::GlobalListCriticalSection);
    OutRunningInstances = FModificationUBTEditorToolInstance::GlobalList;

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerMessageReceived(FString Message)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    AsyncTask(ENamedThreads::GameThread, [SharedSelf, Message] (void)
    {
        SharedSelf->OnWorkerMessageReceived_GameThread(Message);
    });

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerCanceled(void)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    AsyncTask(ENamedThreads::GameThread, [SharedSelf] (void)
    {
        SharedSelf->OnWorkerCancelled_GameThread();
    });

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerCompleted(const int32 ExitCode)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    bool bSuccess   = ExitCode == 0;
    double Duration = this->UATProcess->GetDuration().GetTotalSeconds();

    AsyncTask(ENamedThreads::GameThread, [SharedSelf, bSuccess, Duration, ExitCode] (void)
    {
        SharedSelf->OnWorkerCompleted_GameThread(bSuccess, Duration, ExitCode);
    });

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerMessageReceived_GameThread(const FString& Message)
{
    jcheck( IsInGameThread() )

    ELogVerbosity::Type Verbosity = ELogVerbosity::Display;
    if (Message.Contains(TEXT("Fatal: ")))
    {
        Verbosity = ELogVerbosity::Fatal;
    }
    else if (Message.Contains(TEXT("Error: ")))
    {
        Verbosity = ELogVerbosity::Error;
    }
    else if (Message.Contains(TEXT("Warning: ")))
    {
        Verbosity = ELogVerbosity::Warning;
    }

    this->AddMessageToOutputLog(Verbosity, Message);

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerCancelled_GameThread(void)
{
    jcheck( IsInGameThread() )
    jcheck( this->InstanceState == EModificationUBTEditorToolInstanceState::Running )

    this->InstanceState = EModificationUBTEditorToolInstanceState::Completed;

    this->EndTime = FDateTime::UtcNow();

    this->Result = EModificationUBTEditorToolInstanceResult::Cancelled;
    this->AddMessageToOutputLog(ELogVerbosity::Display, TEXT("Packaging process was cancelled by user."));
    this->MarkNotificationCancelled();

    this->OnProcessCompletedDelegate.Broadcast(this->Result);
    this->RemoveFromGlobalList();

    return;
}

void FModificationUBTEditorToolInstance::OnWorkerCompleted_GameThread(const bool bSuccess, const double Duration, const int32 ExitCode)
{
    jcheck( IsInGameThread() )
    jcheck( this->InstanceState == EModificationUBTEditorToolInstanceState::Running )

    this->InstanceState = EModificationUBTEditorToolInstanceState::Completed;

    if (bSuccess)
    {
        this->Result = EModificationUBTEditorToolInstanceResult::Success;
        this->AddMessageToOutputLog(
            ELogVerbosity::Display,
            FString::Printf(
                TEXT("Packaging process completed in %.2f seconds."),
                Duration
            )
        );
        this->MarkNotificationAsSuccess(Duration);
    }
    else
    {
        this->Result = EModificationUBTEditorToolInstanceResult::Fail;
        this->AddMessageToOutputLog(
            ELogVerbosity::Error,
            FString::Printf(
                TEXT("Packaging process failed after %.2f seconds with exit code %d."),
                Duration,
                ExitCode
            )
        );
        this->MarkNotificationAsFail(ExitCode);
    }

    this->EndTime = FDateTime::UtcNow();

    this->OnProcessCompletedDelegate.Broadcast(this->Result);
    this->RemoveFromGlobalList();

    return;
}

void FModificationUBTEditorToolInstance::HandleCancelButtonClicked(void)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    AsyncTask(ENamedThreads::GameThread, [SharedSelf] (void)
    {
        if (SharedSelf->InstanceState == EModificationUBTEditorToolInstanceState::Running)
        {
            SharedSelf->UATProcess->Cancel();
        }
    });

    return;
}

void FModificationUBTEditorToolInstance::HandleDismissButtonClicked(void)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    AsyncTask(ENamedThreads::GameThread, [SharedSelf] (void)
    {
        SharedSelf->DismissNotification();
    });

    return;
}

void FModificationUBTEditorToolInstance::HandleOutputLogNavigate(void)
{
    const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared();
    AsyncTask(ENamedThreads::GameThread, [SharedSelf] (void)
    {
        if (
            const TSharedPtr<SDockTab> LogTab =
                FGlobalTabmanager::Get()->TryInvokeTab(FTabId(FModificationUBTEditorToolModule::LogTabName));
            LogTab.IsValid()
        )
        {
            const TSharedRef<SModificationUBTEditorToolLogWidget> TabContent =
                StaticCastSharedRef<SModificationUBTEditorToolLogWidget>(LogTab->GetContent());
            TabContent->SetInstance(SharedSelf);
        }

        return;
    });

    return;
}

void FModificationUBTEditorToolInstance::AddMessageToOutputLog(const ELogVerbosity::Type Verbosity, const FString& Message)
{
    jcheck( IsInGameThread() )

    const FModificationUBTEditorToolInstanceMessageEntry MessageEntry { Verbosity, Message };
    this->MessageList.Add(MessageEntry);
    this->OnMessageReceivedDelegate.Broadcast(MessageEntry);

    /* Duplicate the message to the log file, so it can be easily copied from there. */
    FMsg::Logf(
        __FILE__,
        __LINE__,
        LogModificationUBTEditorTool.GetCategoryName(),
        Verbosity,
        TEXT("[Package %s] %s"),
        *this->PluginName,
        *Message
    );

    return;
}

void FModificationUBTEditorToolInstance::SpawnNotification(void)
{
    jcheck( IsInGameThread() )

    FNotificationInfo NotificationInfo { FText::Format(
        LOCTEXT("PackageModTaskName", "Packaging plugin {0}..."), FText::FromString(this->PluginName)
    ) };
    NotificationInfo.bFireAndForget = false;
    NotificationInfo.ExpireDuration = 10.0f;

    NotificationInfo.Hyperlink     = FSimpleDelegate::CreateSP(AsShared(), &FModificationUBTEditorToolInstance::HandleOutputLogNavigate);
    NotificationInfo.HyperlinkText = LOCTEXT("ShowLogHyperlink", "Show Log");

    NotificationInfo.ButtonDetails.Add(
        FNotificationButtonInfo(
            LOCTEXT("NotificationTaskCancel", "Cancel"),
            LOCTEXT("UATTaskCancelToolTip", "Cancels execution of this task."),
            FSimpleDelegate::CreateSP(AsShared(), &FModificationUBTEditorToolInstance::HandleCancelButtonClicked),
            SNotificationItem::CS_Pending
        )
    );
    NotificationInfo.ButtonDetails.Add(
        FNotificationButtonInfo(
            LOCTEXT("NotificationTaskDismiss", "Dismiss"),
            FText(),
            FSimpleDelegate::CreateSP( AsShared(), &FModificationUBTEditorToolInstance::HandleDismissButtonClicked),
            SNotificationItem::CS_Fail
        )
    );

    this->NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
    this->NotificationItem->SetCompletionState( SNotificationItem::CS_Pending );
    if (GEditor)
    {
        GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileStart_Cue.CompileStart_Cue"));
    }
    else
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Global editor reference not found.")
    }

    return;
}

void FModificationUBTEditorToolInstance::DismissNotification(void)
{
    if (this->NotificationItem.IsValid())
    {
        this->NotificationItem->SetExpireDuration( 0.0f );
        this->NotificationItem->SetFadeOutDuration( 0.5f );
        this->NotificationItem->Fadeout();
        this->NotificationItem.Reset();
    }

    return;
}

void FModificationUBTEditorToolInstance::MarkNotificationAsSuccess(const double Duration)
{
    if (this->NotificationItem.IsValid())
    {
        this->NotificationItem->SetText(FText::Format(
            LOCTEXT("PackageModTaskNameSuccess", "Packaged plugin {0}."), FText::FromString(this->PluginName)
        ));
        this->NotificationItem->SetSubText(FText::Format(
            LOCTEXT("PackageModResultSuccess", "Packaging process finished in {0}s."), Duration
        ));
        this->NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
        this->NotificationItem->ExpireAndFadeout();
        this->NotificationItem.Reset();
    }

    if (GEditor)
    {
        GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
    }
    else
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Global editor reference not found.")
    }

    return;
}

void FModificationUBTEditorToolInstance::MarkNotificationAsFail(const int32 ExitCode)
{
    if (this->NotificationItem.IsValid())
    {
        this->NotificationItem->SetText(FText::Format(
            LOCTEXT("PackageModTaskNameFail", "Failed To Package Mod {0}"), FText::FromString(this->PluginName)
        ));
        this->NotificationItem->SetSubText(FText::Format(
            LOCTEXT("PackageModResultFail", "Process Exited with ExitCode {0}, See Log for more details."), ExitCode
        ));
        this->NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
        this->NotificationItem->ExpireAndFadeout();
        this->NotificationItem.Reset();
    }

    if (GEditor)
    {
        GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
    }
    else
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Global editor reference not found.")
    }

    return;
}

void FModificationUBTEditorToolInstance::MarkNotificationCancelled(void)
{
    if (this->NotificationItem.IsValid())
    {
        this->NotificationItem->SetText(FText::Format(
            LOCTEXT("PackagePluginTaskNameCancelled", "Packaging process {0} Cancelled."), FText::FromString(this->PluginName)
        ));
        this->NotificationItem->SetSubText(
            LOCTEXT("PackageModResultCancel", "The packaging process was cancelled by the user."
        ));
        this->NotificationItem->SetCompletionState(SNotificationItem::CS_None);
        this->NotificationItem->ExpireAndFadeout();
        this->NotificationItem.Reset();
    }

    return;
}

void FModificationUBTEditorToolInstance::RegisterInGlobalList(void)
{
    FScopeLock Lock(&FModificationUBTEditorToolInstance::GlobalListCriticalSection);

    if (const TSharedPtr<FModificationUBTEditorToolInstance> SharedSelf = AsShared())
    {
        FModificationUBTEditorToolInstance::GlobalList.AddUnique(SharedSelf);
        FModificationUBTEditorToolInstance::OnNewInstanceSpawnedDelegate.Broadcast(SharedSelf);
    }

    return;
}

void FModificationUBTEditorToolInstance::RemoveFromGlobalList(void)
{
    FScopeLock Lock(&FModificationUBTEditorToolInstance::GlobalListCriticalSection);
    FModificationUBTEditorToolInstance::GlobalList.Remove(AsShared());

    return;
}

#undef LOCTEXT_NAMESPACE
