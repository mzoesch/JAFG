// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChatMessage.h"
#include "JAFGUserWidget.h"

#include "ChatMenu.generated.h"

class UJAFGBorder;
class UJAFGTextBlock;
class UVerticalBox;
class UOverlay;
class UScrollBox;
class UEditableText;
class UJAFGEditableText;
struct FChatCommandObject;

struct CHAT_API FChatMessagePassData final : public FWidgetPassData
{
    FChatMessagePassData() = default;
    explicit FChatMessagePassData(const FChatMessage& InMessage) : Message(InMessage)
    {
    }

    FChatMessage Message;
};

UCLASS(Abstract, Blueprintable)
class CHAT_API UChatMenuEntry : public UJAFGUserWidget
{
    GENERATED_BODY()

public:

    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;

protected:

    FChatMessagePassData MessageData;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_Message;

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

    void AddMessageToChatLog(const EChatMessageType::Type Type, const EChatMessageFormat::Type FormatType, const FString& Sender, const FText& Message);
    void AddMessageToChatLog(const EChatMessageType::Type Type, const FString& Sender, const FText& Message);
    void AddMessageToChatLog(const FString& Sender, const FText& Message);
    void AddMessageToChatLog(const FChatMessage& Message);

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
    auto SafeAddToPreviewOut(const FChatMessage& Message) -> void;
    auto RemoveOutdatedPreviewEntries(void) -> void;
    auto ChangeChatMenuVisibilityStateBasedOnPreviewEntries(void) -> void;
    const float PreviewOutEntryLifetimeInSeconds { 0x5 };
    const int32 MaxEntriesInPreviewOut { 0x8 };
    TArray<FPrivatePreviewEntryData> PreviewEntries;
    auto ConstructChatMenuEntry(const FChatMessage& Message) const -> UChatMenuEntry*;
    /**
     *  The offset of the stdout scroll box to the end, where we scroll to the end if a new message appears and the
     *  stdout is active. If the current offset is higher than this value, we leave the scroll offset as it is.
     */
    const float StdOutScrollOffsetOfEndIgnoreDelta { 0x3F };

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UOverlay> Overlay_OutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PanelWidget_StdOutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UScrollBox> ScrollBox_StdOut;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PanelWidget_PreviewOutWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UVerticalBox> VerticalBox_PreviewOut;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGEditableText> EditableText_StdIn;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UJAFGBorder> Border_StdInWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UPanelWidget> PanelWidget_CmdSuggestionsWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UVerticalBox> VerticalBox_CmdSuggestions;

    void UpdateCmdSuggestions(const FText& InText) const;
    void UpdateCmdSuggestions(void) const;
    void MarkCommandInAsInvalid(void) const;
    void MarkCommandInAsValid(void) const;
    void HideCommandSuggestionsWindow(void) const;
    void ShowCommandSuggestionsWindow(const TArray<FString>& Content, const bool bUpdateStdInValidityFeedback = false) const;
    void ShowCommandSuggestionsWindow(const FChatCommandObject& InObj) const;
    void RerenderCommandSuggestionsWindow(void) const;

    UFUNCTION()
    void OnChatTextCommitted(const FText& Text, const ETextCommit::Type CommitMethod);
    void OnChatTextChanged(const FText& Text);

    static constexpr int32 InvalidCursorInHistory { -1 };
    int32 CurrentCursorInHistory = UChatMenu::InvalidCursorInHistory;
    virtual FOnKeyDown GetOnStdInKeyDownHandler(void);
    void OnHistoryLookUp(const bool bPrevious);
    FDelegateHandle ChatHistoryLookupHandle;
    mutable FText LastSelfTypedIn;

    static constexpr int32 InvalidCursorInCmdSuggestions { 0 };
    mutable int32 CurrentCursorInCmdSuggestions = UChatMenu::InvalidCursorInCmdSuggestions;
    void OnCommandSuggestionLookUp(const bool bPrevious) const;
    void OnFillCommandSuggestion(void) const;
    mutable TArray<FString> CommandSuggestions;
    FDelegateHandle FillCommandSuggestionToChatStdInHandle;

    auto FocusStdIn(void) const -> void;
    auto ClearStdIn(void) const -> void;

    auto IsStdInValid(void) const -> bool;
};
