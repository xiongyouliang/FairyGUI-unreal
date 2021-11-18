#pragma once

#include "CoreMinimal.h"

#include "Tween/FairyTweener.h"

#include "FairyTweenerFiniteTime.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerFiniteTime : public UFairyTweener
{
	GENERATED_BODY()
public:
	UFairyTweenerFiniteTime();
	virtual ~UFairyTweenerFiniteTime();

	float GetDuration() const;
	void SetDuration(float InDuration);

	void Init(float InDuration);

protected:
	float duration;
};