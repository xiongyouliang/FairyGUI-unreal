#include "Tween/FairyTweener.h"
#include "Tween/EaseManager.h"
#include "Tween/GPath.h"
#include "UI/FairyObject.h"

const float K_MATH_EPSILON = 0.000001f;

UFairyTweener::UFairyTweener()
	:_Target(nullptr)
	,_tag(0)
{
}

UFairyTweener::~UFairyTweener()
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyTweener Destructed."));
}

UFairyTweener* UFairyTweener::SetEase(EEaseType InValue)
{
	EaseType = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetEasePeriod(float InValue)
{
	EasePeriod = InValue;
	return this;
}

UFairyTweener* UFairyTweener::SetEaseOvershootOrAmplitude(float InValue)
{
	EaseOvershootOrAmplitude = InValue;
	return this;
}

void UFairyTweener::StartWithTarget(UFairyObject* InTarget)
{
	_Target = InTarget;
}

void UFairyTweener::SetTarget(UFairyObject* InTarget)
{
	_Target = InTarget;
}

UFairyObject* UFairyTweener::GetTarget()
{
	return _Target;
}

bool UFairyTweener::IsTargetValid()
{
	return _Target != nullptr;
}

bool UFairyTweener::IsDone() const
{
	return true;
}

UFairyTweener* UFairyTweener::SetPath(TSharedPtr<FGPath> InPath)
{
	Path = InPath;
	return this;
}

UFairyTweener* UFairyTweener::SetPaused(bool bInPaused)
{
	bPaused = bInPaused;
	return this;
}

bool UFairyTweener::IsPaused()
{
	return bPaused;
}

void UFairyTweener::Step(float DeltaTime)
{

}

void UFairyTweener::Update(float DeltaTime)
{
}

void UFairyTweener::SetTag(int InTag)
{
	_tag = InTag;
}

int UFairyTweener::GetTag()
{
	return _tag;
}