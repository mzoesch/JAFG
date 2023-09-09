// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DBUtils/DataAccessLayer.h"


class UGI_CORE;


class JAFG_API ALVL_CORE {
public:	
	
	ALVL_CORE();
	ALVL_CORE(UGI_CORE* GI);
	~ALVL_CORE();
	
private:

	UGI_CORE* GI;
	
private:
	
	DataAccessLayer* DAL;
	
};
