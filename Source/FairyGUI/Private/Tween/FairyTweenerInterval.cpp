#include "Tween/FairyTweenerInterval.h"

float FFairyTweenerInterval::GetElapsedTime()
{
	return elapsedTime;
}

void FFairyTweenerInterval::SetAmplitudeRate(float InAmplitudeRate)
{

}

float FFairyTweenerInterval::GetAmplitudeRate()
{
	return 0.0f;
}

bool FFairyTweenerInterval::IsDone() const
{
	return bDone;
}

void FFairyTweenerInterval::Step(float InDeltaTime)
{
	if (bFirstTick)
	{
		bFirstTick = false;
		elapsedTime = K_MATH_EPSILON;
	}
	else
	{
		elapsedTime += InDeltaTime;
	}

	float updateTime = FMath::Max(0.0f, FMath::Min(1.0f, elapsedTime / duration));
	Update(updateTime);

	bDone = elapsedTime >= GetDuration();
}

void FFairyTweenerInterval::StartWithTarget(UFairyObject* InTarget)
{
	FFairyTweenerFiniteTime::StartWithTarget(InTarget);
}

void FFairyTweenerInterval::Init(float InDuration)
{
	FFairyTweenerFiniteTime::Init(InDuration);

	elapsedTime = 0.0f;
	bFirstTick = true;
	bDone = false;
}

// Position tweener
FFairyTweenerPos* FFairyTweenerPos::Create(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	FFairyTweenerPos* tweener = new FFairyTweenerPos();
	if (tweener)
	{
		if (tweener->Init(InDuration, InStartPos, InDstPos))
		{
			return tweener;
		}
		else
		{
			delete tweener;
		}
	}
	
	return nullptr;
}

bool FFairyTweenerPos::Init(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	FFairyTweenerInterval::Init(InDuration);

	startPos = InStartPos;
	dstPos = InDstPos;
	return true;
}

void FFairyTweenerPos::Update(float InTime)
{
	if (_Target.IsValid())
	{
		FVector2D curPos = (dstPos - startPos) * InTime;
		_Target->SetPosition(curPos);
	}
}
