// © 2023 mzoesch. All rights reserved.


#include "MainMenu/FLocalSave.h"

FLocalSave::FLocalSave() {
	return;
}

FLocalSave::FLocalSave(FString SaveName, FString LastPlayedDate) {
	this->SaveName = SaveName;
	this->LastPlayedDate = LastPlayedDate;
	
	return;
}

FLocalSave::~FLocalSave() {
	return;
}

