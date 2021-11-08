#pragma once

#include "CoreMinimal.h"
#include "UI/FieldTypes.h"
#include "Tween/GPath.h"
#include "Tween/EaseType.h"
#include "UI/Transition/TransitionItemData.h"

struct FTweenConfig {
	float Duration;
	EEaseType EaseType;
	int32 Repeat;
	bool bYoyo;

	FTransitionItemData StartData;
	FTransitionItemData EndData;
	TSharedPtr<FGPath> Path;

	FString EndLabel;
	FSimpleDelegate EndHook;

	FTweenConfig();
};