#include "Tween/FairyEaseType.h"

float FFairyEaseHelper::EaseRateIn(float InTime, float InRate)
{
	return FMath::Pow(InTime, InRate);
}

float FFairyEaseHelper::EaseRateOut(float InTime, float InRate)
{
	return FMath::Pow(InTime, 1 / InRate);
}

float FFairyEaseHelper::EaseRateInOut(float InTime, float InRate)
{
	InTime *= 2;
	if (InTime < 1)
	{
		return 0.5f * FMath::Pow(InTime, InRate);
	}
	else
	{
		return (1.0f - 0.5f * FMath::Pow(2 - InTime, InRate));
	}
}

float FFairyEaseHelper::EaseLinear(float InTime)
{
	return InTime;
}

float FFairyEaseHelper::SineEaseIn(float InTime)
{
	return -1 * FMath::Cos(InTime * HALF_PI) + 1;
}

float FFairyEaseHelper::SineEaseOut(float InTime)
{
	return FMath::Sin(InTime * HALF_PI);
}

float FFairyEaseHelper::SineEaseInOut(float InTime)
{
	return -0.5f * (FMath::Cos(PI * InTime) - 1);
}

float FFairyEaseHelper::QuadEaseIn(float InTime)
{
	return InTime * InTime;
}

float FFairyEaseHelper::QuadEaseOut(float InTime)
{
	return -1 * InTime * (InTime - 2);
}

float FFairyEaseHelper::QuadEaseInOut(float InTime)
{
	InTime = InTime * 2;
	if (InTime < 1)
	{
		return 0.5f * InTime * InTime;
	}
	else
	{
		--InTime;
		return -0.5f * (InTime * (InTime - 2) - 1);
	}
}

float FFairyEaseHelper::CubicEaseIn(float InTime)
{
	return InTime * InTime * InTime;
}

float FFairyEaseHelper::CubicEaseOut(float InTime)
{
	InTime -= 1;
	return (InTime * InTime * InTime + 1);
}

float FFairyEaseHelper::CubicEaseInOut(float InTime)
{
	InTime = InTime * 2;
	if (InTime < 1)
	{
		return 0.5f * InTime * InTime * InTime;
	}
	else
	{
		InTime -= 2;
		return 0.5f * (InTime * InTime * InTime + 2);
	}
}

float FFairyEaseHelper::QuartEaseIn(float InTime)
{
	return InTime * InTime * InTime * InTime;
}

float FFairyEaseHelper::QuartEaseOut(float InTime)
{
	InTime -= 1;
	return -(InTime * InTime * InTime * InTime - 1);
}

float FFairyEaseHelper::QuartEaseInOut(float InTime)
{
	InTime = InTime * 2;
	if (InTime < 1)
	{
		return 0.5f * InTime * InTime * InTime * InTime;
	}
	else
	{
		InTime -= 2;
		return -0.5f * (InTime * InTime * InTime * InTime - 2);
	}
}

float FFairyEaseHelper::QuintEaseIn(float InTime)
{
	return InTime * InTime * InTime * InTime * InTime;
}

float FFairyEaseHelper::QuintEaseOut(float InTime)
{
	InTime -= 1;
	return (InTime * InTime * InTime * InTime * InTime + 1);
}

float FFairyEaseHelper::QuintEaseInOut(float InTime)
{
	InTime = InTime * 2;
	if (InTime < 1)
	{
		return 0.5f * InTime * InTime * InTime * InTime * InTime;
	}
	{
		InTime -= 2;
		return 0.5f * (InTime * InTime * InTime * InTime * InTime + 2);
	}
}

float FFairyEaseHelper::ExpoEaseIn(float InTime)
{
	return InTime == 0 ? 0 : FMath::Pow(2, 10 * (InTime / 1 - 1)) - 1 * 0.001f;
}

float FFairyEaseHelper::ExpoEaseOut(float InTime)
{
	return InTime == 1 ? 1 : (-FMath::Pow(2, -10 * InTime / 1) + 1);
}

float FFairyEaseHelper::ExpoEaseInOut(float InTime)
{
	if (InTime == 0 || InTime == 1)
	{
		return InTime;
	}

	if (InTime < 0.5f)
	{
		return 0.5f * FMath::Pow(2, 10 * (InTime * 2 - 1));
	}

	return 0.5f * (-FMath::Pow(2, -10 * (InTime * 2 - 1)) + 2);
}


float FFairyEaseHelper::CircEaseIn(float InTime)
{
	return -1 * (FMath::Sqrt(1 - InTime * InTime) - 1);
}

float FFairyEaseHelper::CircEaseOut(float InTime)
{
	InTime = InTime - 1;
	return FMath::Sqrt(1 - InTime * InTime);
}

float FFairyEaseHelper::CircEaseInOut(float InTime)
{
	InTime = InTime * 2;
	if (InTime < 1)
	{
		return -0.5f * (FMath::Sqrt(1 - InTime * InTime) - 1);
	}
	else
	{
		InTime -= 2;
		return 0.5f * (FMath::Sqrt(1 - InTime * InTime) + 1);
	}
}


float FFairyEaseHelper::ElasticEaseIn(float InTime, float InPeriod)
{
	float newT = 0;
	if (InTime == 0 || InTime == 1)
	{
		newT = InTime;
	}
	else
	{
		float s = InPeriod / 4;
		InTime = InTime - 1;
		newT = -powf(2, 10 * InTime) * sinf((InTime - s) * PI * 2 / InPeriod);
	}

	return newT;
}

float FFairyEaseHelper::ElasticEaseOut(float InTime, float InPeriod)
{
	float newT = 0;
	if (InTime == 0 || InTime == 1)
	{
		newT = InTime;
	}
	else
	{
		float s = InPeriod / 4;
		newT = powf(2, -10 * InTime) * sinf((InTime - s) * PI * 2 / InPeriod) + 1;
	}

	return newT;
}

float FFairyEaseHelper::ElasticEaseInOut(float InTime, float InPeriod)
{
	float newT = 0;
	if (InTime == 0 || InTime == 1)
	{
		newT = InTime;
	}
	else
	{
		InTime = InTime * 2;
		if (!InPeriod)
		{
			InPeriod = 0.3f * 1.5f;
		}

		float s = InPeriod / 4;

		InTime = InTime - 1;
		if (InTime < 0)
		{
			newT = -0.5f * powf(2, 10 * InTime) * sinf((InTime - s) * PI * 2 / InPeriod);
		}
		else
		{
			newT = powf(2, -10 * InTime) * sinf((InTime - s) * PI * 2 / InPeriod) * 0.5f + 1;
		}
	}
	return newT;
}

float FFairyEaseHelper::BackEaseIn(float InTime)
{
	float overshoot = 1.70158f;
	return InTime * InTime * ((overshoot + 1) * InTime - overshoot);
}

float FFairyEaseHelper::BackEaseOut(float InTime)
{
	float overshoot = 1.70158f;

	InTime = InTime - 1;
	return InTime * InTime * ((overshoot + 1) * InTime + overshoot) + 1;
}

float FFairyEaseHelper::BackEaseInOut(float InTime)
{
	float overshoot = 1.70158f * 1.525f;

	InTime = InTime * 2;
	if (InTime < 1)
	{
		return (InTime * InTime * ((overshoot + 1) * InTime - overshoot)) / 2;
	}
	else
	{
		InTime = InTime - 2;
		return (InTime * InTime * ((overshoot + 1) * InTime + overshoot)) / 2 + 1;
	}
}


float bounceTime(float InTime)
{
	if (InTime < 1 / 2.75f)
	{
		return 7.5625f * InTime * InTime;
	}
	else if (InTime < 2 / 2.75f)
	{
		InTime -= 1.5f / 2.75f;
		return 7.5625f * InTime * InTime + 0.75f;
	}
	else if (InTime < 2.5f / 2.75f)
	{
		InTime -= 2.25f / 2.75f;
		return 7.5625f * InTime * InTime + 0.9375f;
	}

	InTime -= 2.625f / 2.75f;
	return 7.5625f * InTime * InTime + 0.984375f;
}
float FFairyEaseHelper::BounceEaseIn(float InTime)
{
	return 1 - bounceTime(1 - InTime);
}

float FFairyEaseHelper::BounceEaseOut(float InTime)
{
	return bounceTime(InTime);
}

float FFairyEaseHelper::BounceEaseInOut(float InTime)
{
	float newT = 0;
	if (InTime < 0.5f)
	{
		InTime = InTime * 2;
		newT = (1 - bounceTime(1 - InTime)) * 0.5f;
	}
	else
	{
		newT = bounceTime(InTime * 2 - 1) * 0.5f + 0.5f;
	}

	return newT;
}

float FFairyEaseHelper::CustomEase(float InTime, float* InEasingParam)
{
	if (InEasingParam)
	{
		float tt = 1 - InTime;
		return InEasingParam[1] * tt * tt * tt + 3 * InEasingParam[3] * InTime * tt * tt + 3 * InEasingParam[5] * InTime * InTime * tt + InEasingParam[7] * InTime * InTime * InTime;
	}
	return InTime;
}


float FFairyEaseHelper::EaseTime(EFairyEaseType InEaseType, float InTime, float InPeriod)
{
	switch (InEaseType)
	{
	case EFairyEaseType::Linear:
		return FFairyEaseHelper::EaseLinear(InTime);
	case EFairyEaseType::SineIn:
		return FFairyEaseHelper::SineEaseIn(InTime);
	case EFairyEaseType::SineOut:
		return FFairyEaseHelper::SineEaseOut(InTime);
	case EFairyEaseType::SineInOut:
		return FFairyEaseHelper::SineEaseInOut(InTime);
	case EFairyEaseType::QuadIn:
		return FFairyEaseHelper::QuadEaseIn(InTime);
	case EFairyEaseType::QuadOut:
		return FFairyEaseHelper::QuadEaseOut(InTime);
	case EFairyEaseType::QuadInOut:
		return FFairyEaseHelper::QuadEaseInOut(InTime);
	case EFairyEaseType::CubicIn:
		return FFairyEaseHelper::CubicEaseIn(InTime);
	case EFairyEaseType::CubicOut:
		return FFairyEaseHelper::CubicEaseOut(InTime);
	case EFairyEaseType::CubicInOut:
		return FFairyEaseHelper::CubicEaseInOut(InTime);
	case EFairyEaseType::QuartIn:
		return FFairyEaseHelper::QuartEaseIn(InTime);
	case EFairyEaseType::QuartOut:
		return FFairyEaseHelper::QuartEaseOut(InTime);
	case EFairyEaseType::QuartInOut:
		return FFairyEaseHelper::QuartEaseInOut(InTime);
	case EFairyEaseType::QuintIn:
		return FFairyEaseHelper::QuintEaseIn(InTime);
	case EFairyEaseType::QuintOut:
		return FFairyEaseHelper::QuintEaseOut(InTime);
	case EFairyEaseType::QuintInOut:
		return FFairyEaseHelper::QuintEaseInOut(InTime);
	case EFairyEaseType::ExpoIn:
		return FFairyEaseHelper::ExpoEaseIn(InTime);
	case EFairyEaseType::ExpoOut:
		return FFairyEaseHelper::ExpoEaseOut(InTime);
	case EFairyEaseType::ExpoInOut:
		return FFairyEaseHelper::ExpoEaseInOut(InTime);
	case EFairyEaseType::CircIn:
		return FFairyEaseHelper::CircEaseIn(InTime);
	case EFairyEaseType::CircOut:
		return FFairyEaseHelper::CircEaseOut(InTime);
	case EFairyEaseType::CircInOut:
		return FFairyEaseHelper::CircEaseInOut(InTime);
	case EFairyEaseType::ElasticIn:
		return FFairyEaseHelper::ElasticEaseIn(InTime, InPeriod);
	case EFairyEaseType::ElasticOut:
		return FFairyEaseHelper::ElasticEaseOut(InTime, InPeriod);
	case EFairyEaseType::ElasticInOut:
		return FFairyEaseHelper::ElasticEaseInOut(InTime, InPeriod);
	case EFairyEaseType::BackIn:
		return FFairyEaseHelper::BackEaseIn(InTime);
	case EFairyEaseType::BackOut:
		return FFairyEaseHelper::BackEaseOut(InTime);
	case EFairyEaseType::BackInOut:
		return FFairyEaseHelper::BackEaseInOut(InTime);
	case EFairyEaseType::BounceIn:
		return FFairyEaseHelper::BounceEaseIn(InTime);
	case EFairyEaseType::BounceOut:
		return FFairyEaseHelper::BounceEaseOut(InTime);
	case EFairyEaseType::BounceInOut:
		return FFairyEaseHelper::BounceEaseInOut(InTime);
	default:
		return FFairyEaseHelper::QuadEaseIn(InTime);
	}
}
