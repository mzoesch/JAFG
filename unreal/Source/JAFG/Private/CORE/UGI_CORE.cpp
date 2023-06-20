// © 2023 mzoesch. All rights reserved.


#include "CORE/UGI_CORE.h"

#include "MainMenu/APC_MainMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


UGI_CORE::UGI_CORE()
{
}

UGI_CORE::~UGI_CORE()
{
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
		if (this->TargetOwningPlayerController)
			this->TargetOwningPlayerController->ShowLoadingScreen(
				FText::FromString("Loading Map...")
			);
		GetWorld()->ServerTravel("/Game/Levels/Dev?listen");
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to create session (%s)."), *SessionName.ToString());
	if (this->TargetOwningPlayerController)
		this->TargetOwningPlayerController->ShowErrorMessage(
			FText::FromString("Failed to create session.")
		);
	return;
}

void UGI_CORE::OnFindSessionComplete(bool bSucceeded) {
	if (bSucceeded) {
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		if (SearchResults.Num()) {
			SessionInterface->JoinSession(0, FName("Some Generic Session"), SearchResults[0]);
		}
	}

	return;
}

void UGI_CORE::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "") {
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}

	return;
}

#pragma region BP API

void UGI_CORE::CreateServer(
	APC_MainMenu* OwningPlayerController,
	int MaxPublicConnections,
	FText SessionName
) {
	this->TargetOwningPlayerController = OwningPlayerController;
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

	
	FName BaseSessionName = "JAFG-Session";
	FString Converted = SessionName.ToString();
	Converted.ReplaceInline(TEXT(" "), TEXT("-"));
	FName FullSessionName = FName(
		*FString::Printf(TEXT("%s-%s"),
			*BaseSessionName.ToString(),
			*Converted
			)
		)
		;

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

#pragma endregion
