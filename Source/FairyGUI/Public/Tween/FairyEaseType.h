#pragma once

#include "FairyEaseType.generated.h"

UENUM()
enum class EFairyEaseType : uint8
{
	Linear = 0,

	SineIn,
	SineOut,
	SineInOut,

	QuadIn,
	QuadOut,
	QuadInOut,

	CubicIn,
	CubicOut,
	CubicInOut,

	QuartIn,
	QuartOut,
	QuartInOut,

	QuintIn,
	QuintOut,
	QuintInOut,

	ExpoIn,
	ExpoOut,
	ExpoInOut,

	CircIn,
	CircOut,
	CircInOut,

	ElasticIn,
	ElasticOut,
	ElasticInOut,

	BackIn,
	BackOut,
	BackInOut,

	BounceIn,
	BounceOut,
	BounceInOut,

	RateIn,
	RateOut,
	RateInOut,

	Custom
};

class FFairyEaseHelper
{
public:
	static float EaseRateIn(float InTime, float InRate);
	static float EaseRateOut(float InTime, float InRate);
	static float EaseRateInOut(float InTime, float InRate);

	static float EaseLinear(float InTime);
	
	static float SineEaseIn(float InTime);
	static float SineEaseOut(float InTime);
	static float SineEaseInOut(float InTime);

	static float QuadEaseIn(float InTime);
	static float QuadEaseOut(float InTime);
	static float QuadEaseInOut(float InTime);

	static float CubicEaseIn(float InTime);
	static float CubicEaseOut(float InTime);
	static float CubicEaseInOut(float InTime);

	static float QuartEaseIn(float InTime);
	static float QuartEaseOut(float InTime);
	static float QuartEaseInOut(float InTime);

	static float QuintEaseIn(float InTime);
	static float QuintEaseOut(float InTime);
	static float QuintEaseInOut(float InTime);

	static float ExpoEaseIn(float InTime);
	static float ExpoEaseOut(float InTime);
	static float ExpoEaseInOut(float InTime);

	static float CircEaseIn(float InTime);
	static float CircEaseOut(float InTime);
	static float CircEaseInOut(float InTime);

	static float ElasticEaseIn(float InTime, float InPeriod);
	static float ElasticEaseOut(float InTime, float InPeriod);
	static float ElasticEaseInOut(float InTime, float InPeriod);

	static float BackEaseIn(float InTime);
	static float BackEaseOut(float InTime);
	static float BackEaseInOut(float InTime);

	static float BounceEaseIn(float InTime);
	static float BounceEaseOut(float InTime);
	static float BounceEaseInOut(float InTime);

	static float CustomEase(float InTime, float* InEasingParam);

	static float EaseTime(EFairyEaseType InEaseType, float InTime, float InPeriod = 0.0f);
};