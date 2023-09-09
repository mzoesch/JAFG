// © 2023 mzoesch. All rights reserved.


#include "CORE/UGI_CORE.h"

#include "MainMenu/APC_MainMenu.h"
#include "MainMenu/FSearchResult.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


UGI_CORE::UGI_CORE()
{
}

UGI_CORE::~UGI_CORE() {
	UE_LOG(LogTemp, Warning, TEXT("Trying to kill session %s."), *this->SessionWorldURL)

	const FName BaseSessionName = "JAFG-Session";
	FString Converted = this->SessionWorldURL;
	Converted.ReplaceInline(TEXT(" "), TEXT("-"));
	const FName FullSessionName = FName(
		*FString::Printf(TEXT("%s-%s"),
			*BaseSessionName.ToString(),
			*Converted
			)
		)
		;
	
	if (SessionInterface.IsValid()) {
		SessionInterface->DestroySession(FullSessionName);
	}

	return;
}

void UGI_CORE::Init() {
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) {
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGI_CORE::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGI_CORE::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGI_CORE::OnJoinSessionComplete);
		}
	}

	return;
}

void UGI_CORE::OnCreateSessionComplete(FName SessionName, bool bSucceeded) {
	if (bSucceeded) {
		UE_LOG(LogTemp, Warning, TEXT("Successfully created session (%s). Loading Map."), *SessionName.ToString());
		if (this->_CallbackTarget)
			this->_CallbackTarget->ShowLoadingScreen(
				FText::FromString("Loading Map...")
			);
		GetWorld()->ServerTravel("/Game/Levels/CORE?listen");
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to create session (%s)."), *SessionName.ToString());
	if (this->_CallbackTarget)
		this->_CallbackTarget->ShowErrorMessage(
			FText::FromString("Failed to create session.")
		);
	return;
}

void UGI_CORE::OnFindSessionComplete(bool bSucceeded) {
	if (bSucceeded) {
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		TArray<FSearchResult> ProcessedSearchResults;
		for (int i = 0; i < SearchResults.Num(); i++) {
			FSearchResult Result;
			Result.SessionName = SearchResults[i].Session.OwningUserName;
			Result.MaxPublicConnections = SearchResults[i].Session.SessionSettings.NumPublicConnections;
			Result.CurrentPublicConnections =
				Result.MaxPublicConnections - SearchResults[i].Session.NumOpenPublicConnections;
			Result.PingInMs = SearchResults[i].PingInMs;
			ProcessedSearchResults.Add(Result);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Successfully found %s sessions."), *FString::FromInt(SearchResults.Num()));
		this->_CallbackTarget->OnSearchSessionsComplete(ProcessedSearchResults);
		
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to find sessions."));
	if (this->_CallbackTarget)
		this->_CallbackTarget->ShowErrorMessage(
			FText::FromString("Failed to find sessions.")
	);
	return;
}

void UGI_CORE::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "") {
			UE_LOG(LogTemp, Warning, TEXT("Successfully joined session (%s). Traveling to Session."), *SessionName.ToString());
			this->_CallbackTarget->ShowLoadingScreen(
				FText::FromString("Traveling to Session...")
			);
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
			
			return;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to join session (%s)."), *SessionName.ToString());
	this->_CallbackTarget->ShowErrorMessage(
		FText::FromString("Failed to join session.")
	);

	return;
}

#pragma region BP API

void UGI_CORE::CreateServer(
	APC_MainMenu* CallbackTarget,
	const int MaxPublicConnections,
	const FText SessionName
) {
	this->_CallbackTarget = CallbackTarget;
	UE_LOG(LogTemp, Warning,
		TEXT("Creating listen Server with %s public connections."),
		*FString::FromInt(MaxPublicConnections)
	);

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = MaxPublicConnections;

	
	const FName BaseSessionName = "JAFG-Session";
	FString Converted = SessionName.ToString();
	Converted.ReplaceInline(TEXT(" "), TEXT("-"));
	const FName FullSessionName = FName(
		*FString::Printf(TEXT("%s-%s"),
			*BaseSessionName.ToString(),
			*Converted
			)
		)
		;
	this->SessionWorldURL = Converted;

	SessionInterface->CreateSession(0, FullSessionName, SessionSettings);

	return;
}

void UGI_CORE::JoinServer() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	return;
}

void UGI_CORE::FindServers(APC_MainMenu* CallbackTarget) {
	this->_CallbackTarget = CallbackTarget;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 50;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	return;
}

void UGI_CORE::JoinServerByIndex(APC_MainMenu* CallbackTarget, int Index) {
	this->_CallbackTarget = CallbackTarget;
	
	bool bSuccess = SessionInterface->JoinSession(
		0,
		FName(*SessionSearch->SearchResults[Index].GetSessionIdStr()),
		SessionSearch->SearchResults[Index]
	);

	if (bSuccess)
		return;

	UE_LOG(LogTemp, Error, TEXT("Failed to join session (%s)."), *SessionSearch->SearchResults[Index].GetSessionIdStr());
	this->_CallbackTarget->ShowErrorMessage(
		FText::FromString("Failed to join session.")
	);
		
	return;
}

#pragma endregion
