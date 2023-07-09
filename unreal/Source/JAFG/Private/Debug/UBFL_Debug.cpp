// © 2023 mzoesch. All rights reserved.


#include "Debug/UBFL_Debug.h"

#include "World/FJAFGCoordinateSystem.h"
#include "World/FJAFGCoordinateSystemPrecise.h"

#include "CORE/APC_CORE.h"
#include "Debug/DebugUtils.h"

#pragma region Debug API

FString UBFL_Debug::ProjectID() {
	FString AppID;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectID"),
		AppID,
		GGameIni
	);

	return AppID;
}

FString UBFL_Debug::ProjectIDFormatted() {
	FString APPID = ProjectID();

	APPID = "{" + APPID;
	APPID.AppendChar('}');

	APPID.InsertAt(9, '-');
	APPID.InsertAt(14, '-');
	APPID.InsertAt(19, '-');
	APPID.InsertAt(24, '-');

	return APPID;
}

FString UBFL_Debug::ProjectVersionNumber() {
	FString AppVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		AppVersion,
		GGameIni
	);

	return AppVersion;
}

FString UBFL_Debug::ProjectName() {
	FString AppName;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectName"),
		AppName,
		GGameIni
	);

	return AppName;
}

FString UBFL_Debug::ProjectDescription() {
	FString AppDescription;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("Description"),
		AppDescription,
		GGameIni
	);

	return AppDescription;
}

FString UBFL_Debug::ProjectCompanyName() {
	FString AppCompanyName;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("CompanyName"),
		AppCompanyName,
		GGameIni
	);

	return AppCompanyName;
}

FString UBFL_Debug::ProjectHomePage() {
	FString AppHomePage;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("Homepage"),
		AppHomePage,
		GGameIni
	);

	return AppHomePage;
}

#pragma endregion Debug API

#pragma region Debug Screen Sections

FString UBFL_Debug::DebugScreenSectionOne_FPS() {
	float DeltaTime = GWorld->GetDeltaSeconds();
	FString DeltaTimeFormatted = FString::SanitizeFloat(DeltaTime * 1000, 2);
	DeltaTimeFormatted.RemoveAt(
		DeltaTimeFormatted.Find(TEXT(".")) + 3,
		DeltaTimeFormatted.Len() - (DeltaTimeFormatted.Find(TEXT(".")) + 3)
	);

	return FString::Printf(
		TEXT("%s fps @ %s ms T: %s GPU: N/A"),
		*FString::FromInt(FMath::FloorToInt(DebugUtils::GAverageFPS())),
		*DeltaTimeFormatted,
		*FString::SanitizeFloat(DebugUtils::GetMaxFPS())
	);
}

FString UBFL_Debug::DebugScreenSectionTwo_LocationPrecise(APC_CORE* PC_Core) {
	return
		FJAFGCoordinateSystemPrecise(
			PC_Core->GetPlayerPosition()
		).ToString();
}

FString UBFL_Debug::DebugScreenSectionTwo_Location(APC_CORE* PC_Core) {
	return
		FJAFGCoordinateSystem(
			PC_Core->GetPlayerPosition()
		).ToString();
}

FString UBFL_Debug::DebugScreenSectionTwo_Facing(APC_CORE* PC_Core) {
	float Yaw = PC_Core->GetPlayerYaw();
	
	// Yaw is clamped from -180 to 180
	FString YawAsText = TEXT("N/A");
	if (Yaw >= -45.f && Yaw <= 45.f)
		YawAsText = TEXT("north (Towards positive X)");
	else if (Yaw > 45.f && Yaw < 135.f)
		YawAsText = TEXT("east (Towards positive Y)");
	else if (Yaw >= 135.f || Yaw <= -135.f)
		YawAsText = TEXT("south (Towards negative X)");
	else if (Yaw > -135.f && Yaw < -45.f)
		YawAsText = TEXT("west (Towards negative Y)");

	return FString::Printf(
		TEXT("Facing: %s (%s)"),
		*YawAsText,
		*FString::SanitizeFloat(Yaw)
	);
}

FString UBFL_Debug::DebugScreenSectionTwo_ChunkLocation(APC_CORE* PC_Core) {
	return FString();
}

#pragma endregion Debug Screen Sections
