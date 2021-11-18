#include "Tween/FairyTweenerInterval.h"

float UFairyTweenerInterval::GetElapsedTime()
{
	return elapsedTime;
}

void UFairyTweenerInterval::SetAmplitudeRate(float InAmplitudeRate)
{

}

float UFairyTweenerInterval::GetAmplitudeRate()
{
	return 0.0f;
}

bool UFairyTweenerInterval::IsDone() const
{
	return bDone;
}

void UFairyTweenerInterval::Step(float InDeltaTime)
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

void UFairyTweenerInterval::StartWithTarget(UFairyObject* InTarget)
{
	UFairyTweenerFiniteTime::StartWithTarget(InTarget);
}

void UFairyTweenerInterval::Init(float InDuration)
{
	UFairyTweenerFiniteTime::Init(InDuration);

	elapsedTime = 0.0f;
	bFirstTick = true;
	bDone = false;
}

// Position tweener
bool UFairyTweenerPos::Init(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	UFairyTweenerInterval::Init(InDuration);

	startPos = InStartPos;
	dstPos = InDstPos;
	return true;
}

void UFairyTweenerPos::Update(float InTime)
{
	UE_LOG(LogTemp, Warning, TEXT("---------> UFairyTweenerPos::Update(), %f"), InTime);
	if (_Target.IsValid())
	{
		FVector2D newPos = startPos + (dstPos - startPos) * InTime;
		_Target->SetPosition(newPos);
	}
}
