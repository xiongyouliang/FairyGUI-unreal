#pragma once

#include "CoreMinimal.h"

#include "Tween/FairyTweener.h"

class FAIRYGUI_API FFairyTweenerFiniteTime : public FFairyTweener
{
public:
	FFairyTweenerFiniteTime();
	virtual ~FFairyTweenerFiniteTime();

	float GetDuration() const;
	void SetDuration(float InDuration);

	void Init(float InDuration);

protected:
	float duration;

private:
	FFairyTweenerFiniteTime(const FFairyTweenerFiniteTime&) = delete;
	FFairyTweenerFiniteTime& operator=(const FFairyTweenerFiniteTime&) = delete;
};