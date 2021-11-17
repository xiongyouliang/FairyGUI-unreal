#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenerFiniteTime.h"

class FAIRYGUI_API FFairyTweenerInterval : public FFairyTweenerFiniteTime
{
public:
	float GetElapsedTime();

	virtual void SetAmplitudeRate(float InAmplitudeRate);
	virtual float GetAmplitudeRate();

	virtual bool IsDone() const override;
	virtual void Step(float InDeltaTime) override;
	virtual void StartWithTarget(UFairyObject* InTarget) override;

	void Init(float InDuration);

protected:
	float elapsedTime;
	bool bFirstTick;
	bool bDone;
};

class FAIRYGUI_API FFairyTweenerPos : public FFairyTweenerInterval
{
public:
	static FFairyTweenerPos* Create(float InDuration, FVector2D InStartPos, FVector2D InDstPos);
	bool Init(float InDuration, FVector2D InStartPos, FVector2D InDstPos);

	virtual void Update(float InTime) override;

protected:
	FVector2D startPos;
	FVector2D dstPos;
};