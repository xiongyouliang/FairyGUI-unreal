#include "Tween/FairyTweenerFiniteTime.h"

FFairyTweenerFiniteTime::FFairyTweenerFiniteTime()
	:duration(0)
{

}

FFairyTweenerFiniteTime::~FFairyTweenerFiniteTime()
{

}

float FFairyTweenerFiniteTime::GetDuration() const
{
	return duration;
}

void FFairyTweenerFiniteTime::SetDuration(float InDuration)
{
	duration = InDuration;
}

void FFairyTweenerFiniteTime::Init(float InDuration)
{
	duration = InDuration;
}