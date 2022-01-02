#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"

struct FFariyTransitionTweenConfig {
	float Duration;
	EFairyEaseType EaseType;
	int32 Repeat;
	bool bYoyo;
};