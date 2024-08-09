// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGUserWidget.h"

#include "ChatMenu.generated.h"

class UVerticalBox;
class UOverlay;
class UTextBlock;
class UScrollBox;
class UEditableText;
class UJAFGEditableText;

struct CHAT_API FChatMessageData final : public FWidgetPassData
{
    FChatMessageData() = default;
    FChatMessageData(const FString& Sender, const FText& Message) : Sender(Sender), Message(Message)
    {
    }

    FString Sender;
    FText   Message;
};

UCLASS(Abstract, Blueprintable)
class CHAT_API UChatMenuEntry : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    FChatMessageData MessageData;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UTextBlock* TB_Message;

    virtual void ConstructMessage(void);
};

namespace EChatMenuVisibility
{

enum Type
{
    Collapsed,
    Preview,
    Full,
};

}

UCLASS(Abstract, Blueprintable)
class CHAT_API UChatMenu : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    explicit UChatMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeDestruct(void) -> void override;
    virtual auto NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) -> void override;
    // ~UUserWidget implementation

public:

    void AddMessageToChatLog(const FString& Sender, const FText& Message);
    void ClearAllChatEntries(void);

protected:

    FDelegateHandle ChatMenuVisibilityChangedHandle;
    virtual auto OnChatMenuVisibilityChanged(const bool bVisible) -> void;
    /**
     * If the chat menu decided on its own to hide again and not through the enhanced input subsystem.
     * E.g.: Due to text commited.
     */
    virtual auto HideChatMenu(void) -> void;

    void ChangeChatMenuVisibility(const EChatMenuVisibility::Type InVisibility);
    auto GetChatMenuVisibility(void) const -> EChatMenuVisibility::Type;

    struct FPrivatePreviewEntryData
    {
        float CreationTimeInSeconds;
        UUserWidget* Entry;
    };
    /** Only adds an entry to the preview out if allowed by current widget state. */
    auto SafeAddToPreviewOut(const FString& Sender, const FText& Message) -> void;
    auto RemoveOutdatedPreviewEntries(void) -> void;
    auto ChangeChatMenuVisibilityStateBasedOnPreviewEntries(void) -> void;
    const float PreviewOutEntryLifetimeInSeconds { 0x5 };
    const int32 MaxEntriesInPreviewOut { 0x8 };
    TArray<FPrivatePreviewEntryData> PreviewEntries;
    auto ConstructChatMenuEntry(const FString& Sender, const FText& Message) const -> UChatMenuEntry*;
    /**
     *  The offset of the stdout scroll box to the end, where we scroll to the end if a new message appears and the
     *  stdout is active. If the current offset is higher than this value, we leave the scroll offset as it is.
     */
    const float StdOutScrollOffsetOfEndIgnoreDelta { 0x3F };

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UOverlay> O_OutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PW_StdOutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UScrollBox> SB_StdOut;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PW_PreviewOutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UVerticalBox> VB_PreviewOut;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEditableText> ET_StdIn;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PW_StdInWrapper;

    UFUNCTION()
    void OnChatTextChanged(const FText& Text);

    UFUNCTION()
    void OnChatTextCommitted(const FText& Text, const ETextCommit::Type CommitMethod);

    static constexpr int32 InvalidCursorInHistory { -1 };
    int32 CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
    virtual FOnKeyDown GetOnStdInKeyDownHandler(void);
    virtual void OnHistoryLookUp(const bool bPrevious);
    FDelegateHandle ChatHistoryLookupHandle;

    auto FocusStdIn(void) const -> void;
    auto ClearStdIn(void) const -> void;

    auto IsStdInValid(void) const -> bool;
};
