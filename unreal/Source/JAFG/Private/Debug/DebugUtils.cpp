// © 2023 mzoesch. All rights reserved.


#include "Debug/DebugUtils.h"

float DebugUtils::GAverageFPS() {
	extern ENGINE_API float GAverageFPS;
	return GAverageFPS;
}

float DebugUtils::GetMaxFPS() {
	if (UEngine* Engine = Cast<UEngine>(GEngine))
		return Engine->GetMaxFPS();

	return 0.0f;
}

void DebugUtils::SetMaxFPS(float MaxFPS) {
	if (UEngine* Engine = Cast<UEngine>(GEngine))
		return Engine->SetMaxFPS(MaxFPS);

	return;
}
