#include "UI/Tween/FairyTweenerFiniteTime.h"

UFairyTweenerFiniteTime::UFairyTweenerFiniteTime()
	:duration(0)
{

}

UFairyTweenerFiniteTime::~UFairyTweenerFiniteTime()
{

}

bool UFairyTweenerFiniteTime::IsInstant()
{
	return false;
}

float UFairyTweenerFiniteTime::GetDuration() const
{
	return duration;
}

void UFairyTweenerFiniteTime::SetDuration(float InDuration)
{
	duration = InDuration;
}

void UFairyTweenerFiniteTime::Init(float InDuration)
{
	duration = InDuration;
}