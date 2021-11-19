#include "Tween/FairyTweenerInstant.h"

void UFairyTweenerInstant::StartWithTarget(UFairyObject* InTarget)
{
	UFairyTweenerFiniteTime::StartWithTarget(InTarget);
	bDone = false;
}

bool UFairyTweenerInstant::IsDone() const
{
	return bDone;
}

void UFairyTweenerInstant::Step(float InDeltaTime)
{
	float updateDt = 1.0f;
	Update(updateDt);
}

void UFairyTweenerInstant::Update(float InTime)
{
	bDone = true;
}