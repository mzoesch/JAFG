// © 2023 mzoesch. All rights reserved.


#include "DBUtils/DBCORE.h"
#include "DBUtils/DataAccessLayer.h"

void DBCORE::TEMP_QUERY() {
	DBCORE::Runs++;
	if (DBCORE::Runs > 1)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("TEMP_QUERY"));
	
	DataAccessLayer DAL = DataAccessLayer(
		DBCORE::DataBaseFolder + TEXT("test.db")
	);
	DAL.TempExecute();

	return;
}
