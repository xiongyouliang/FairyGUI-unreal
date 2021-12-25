#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyEaseType.h"

struct FFariyTransitionTweenConfig {
	float Duration;
	EFairyEaseType EaseType;
	int32 Repeat;
	bool bYoyo;
};