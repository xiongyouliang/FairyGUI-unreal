#include "Tween/FairyTweenerEase.h"

bool UFairyTweenerEase::Init(UFairyTweenerInterval* InInnerTweener)
{
	Super::Init(InInnerTweener->GetDuration());
	_innerTweener = InInnerTweener;
	return true;
}

void UFairyTweenerEase::StartWithTarget(UFairyObject* InTarget)
{
	if (InTarget && _innerTweener)
	{
		UFairyTweenerInterval::StartWithTarget(InTarget);
		_innerTweener->StartWithTarget(InTarget);
	}
}

void UFairyTweenerEase::Update(float InTime)
{
	_innerTweener->Update(InTime);
}

UFairyTweenerInterval* UFairyTweenerEase::GetInnerTweener()
{
	return _innerTweener;
}

void UFairyTweenerEaseRate::SetRate(float InRate)
{
	_rate = InRate;
}

float UFairyTweenerEaseRate::GetRate()
{
	return _rate;
}

bool UFairyTweenerEaseRate::Init(UFairyTweenerInterval* InInnerTweener, float InRate)
{
	if (UFairyTweenerEase::Init(InInnerTweener))
	{
		_rate = InRate;
		return true;
	}
	
	return false;
}

void UFairyTweenerEaseRateIn::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::EaseRateIn(InTime, _rate));
}

void UFairyTweenerEaseRateOut::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::EaseRateOut(InTime, _rate));
}

void UFairyTweenerEaseRateInOut::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::EaseRateInOut(InTime, _rate));
}

void UFairyTweenerEaseElastic::SetPeriod(float InPeriod)
{
	_period = InPeriod;
}

float UFairyTweenerEaseElastic::GetPeriod()
{
	return _period;
}

bool UFairyTweenerEaseElastic::Init(UFairyTweenerInterval* InInnerTweener, float InPeriod)
{
	if (UFairyTweenerEase::Init(InInnerTweener))
	{
		_period = InPeriod;
		return true;
	}
	return false;
}

void UFairyTweenerEaseElasticIn::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::ElasticEaseIn(InTime, _period));
}

void UFairyTweenerEaseElasticOut::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::ElasticEaseOut(InTime, _period));
}

void UFairyTweenerEaseElasticInOut::Update(float InTime)
{
	_innerTweener->Update(FFairyEaseHelper::ElasticEaseInOut(InTime, _period));
}

#define EASE_TEMPLATE_IMP(CLASS_NAME, EASE_FUNC) \
void CLASS_NAME::Update(float InTime) \
{ \
	_innerTweener->Update(FFairyEaseHelper::EASE_FUNC(InTime)); \
}

//void UFairyTweenerEaseSineIn::Update(float InTime)
//{
//	_innerTweener->Update(FFairyEaseHelper::SineEaseIn(InTime));
//}

EASE_TEMPLATE_IMP(UFairyTweenerEaseSineIn, FFairyEaseHelper::SineEaseIn);
EASE_TEMPLATE_IMP(UFairyTweenerEaseSineOut, FFairyEaseHelper::SineEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseSineInOut, FFairyEaseHelper::SineEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseQuadIn, FFairyEaseHelper::QuadEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuadOut, FFairyEaseHelper::QuadEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuadInOut, FFairyEaseHelper::QuadEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseCubicIn, FFairyEaseHelper::CubicEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseCubicOut, FFairyEaseHelper::CubicEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseCubicInOut, FFairyEaseHelper::CubicEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseQuartIn, FFairyEaseHelper::QuartEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuartOut, FFairyEaseHelper::QuartEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuartInOut, FFairyEaseHelper::QuartEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseQuintIn, FFairyEaseHelper::QuintEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuintOut, FFairyEaseHelper::QuintEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseQuintInOut, FFairyEaseHelper::QuintEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseExpoIn, FFairyEaseHelper::ExpoEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseExpoOut, FFairyEaseHelper::ExpoEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseExpoInOut, FFairyEaseHelper::ExpoEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseCircIn, FFairyEaseHelper::CircEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseCircOut, FFairyEaseHelper::CircEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseCircInOut, FFairyEaseHelper::CircEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseBackIn, FFairyEaseHelper::BackEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseBackOut, FFairyEaseHelper::BackEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseBackInOut, FFairyEaseHelper::BackEaseInOut)

EASE_TEMPLATE_IMP(UFairyTweenerEaseBounceIn, FFairyEaseHelper::BounceEaseIn)
EASE_TEMPLATE_IMP(UFairyTweenerEaseBounceOut, FFairyEaseHelper::BounceEaseOut)
EASE_TEMPLATE_IMP(UFairyTweenerEaseBounceInOut, FFairyEaseHelper::BounceEaseInOut)


