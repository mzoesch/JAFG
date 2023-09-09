// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"

#include "UGI_CORE.generated.h"


class APC_MainMenu;


UCLASS()
class JAFG_API UGI_CORE : public UGameInstance {
	GENERATED_BODY()

public:

	UGI_CORE();
	~UGI_CORE();

protected:

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool bSucceeded);
	virtual void OnFindSessionComplete(bool bSucceeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(
		APC_MainMenu* CallbackTarget,
		const int MaxPublicConnections,
		const FText SessionName
	);
	APC_MainMenu* _CallbackTarget;

	UFUNCTION(BlueprintCallable)
	void JoinServer();
	
	UFUNCTION(BlueprintCallable)
	void FindServers(
		APC_MainMenu* CallbackTarget
	);

	UFUNCTION(BlueprintCallable)
	void JoinServerByIndex(
		APC_MainMenu* CallbackTarget,
		int Index
	);

private:
	
	FString SessionWorldURL = "";

public:

	FString GetSessionName() const {
		if (!this->SessionWorldURL.IsEmpty())
			return this->SessionWorldURL;
		else
			return FString("InternalSessionWorldURL");
	}
	
};
