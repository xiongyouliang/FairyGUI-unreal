#include "Tween/EaseType.h"

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


float FFairyEaseHelper::EaseTime(EEaseType InEaseType, float InTime, float InPeriod)
{
	switch (InEaseType)
	{
	case EEaseType::Linear:
		return FFairyEaseHelper::EaseLinear(InTime);
	case EEaseType::SineIn:
		return FFairyEaseHelper::SineEaseIn(InTime);
	case EEaseType::SineOut:
		return FFairyEaseHelper::SineEaseOut(InTime);
	case EEaseType::SineInOut:
		return FFairyEaseHelper::SineEaseInOut(InTime);
	case EEaseType::QuadIn:
		return FFairyEaseHelper::QuadEaseIn(InTime);
	case EEaseType::QuadOut:
		return FFairyEaseHelper::QuadEaseOut(InTime);
	case EEaseType::QuadInOut:
		return FFairyEaseHelper::QuadEaseInOut(InTime);
	case EEaseType::CubicIn:
		return FFairyEaseHelper::CubicEaseIn(InTime);
	case EEaseType::CubicOut:
		return FFairyEaseHelper::CubicEaseOut(InTime);
	case EEaseType::CubicInOut:
		return FFairyEaseHelper::CubicEaseInOut(InTime);
	case EEaseType::QuartIn:
		return FFairyEaseHelper::QuartEaseIn(InTime);
	case EEaseType::QuartOut:
		return FFairyEaseHelper::QuartEaseOut(InTime);
	case EEaseType::QuartInOut:
		return FFairyEaseHelper::QuartEaseInOut(InTime);
	case EEaseType::QuintIn:
		return FFairyEaseHelper::QuintEaseIn(InTime);
	case EEaseType::QuintOut:
		return FFairyEaseHelper::QuintEaseOut(InTime);
	case EEaseType::QuintInOut:
		return FFairyEaseHelper::QuintEaseInOut(InTime);
	case EEaseType::ExpoIn:
		return FFairyEaseHelper::ExpoEaseIn(InTime);
	case EEaseType::ExpoOut:
		return FFairyEaseHelper::ExpoEaseOut(InTime);
	case EEaseType::ExpoInOut:
		return FFairyEaseHelper::ExpoEaseInOut(InTime);
	case EEaseType::CircIn:
		return FFairyEaseHelper::CircEaseIn(InTime);
	case EEaseType::CircOut:
		return FFairyEaseHelper::CircEaseOut(InTime);
	case EEaseType::CircInOut:
		return FFairyEaseHelper::CircEaseInOut(InTime);
	case EEaseType::ElasticIn:
		return FFairyEaseHelper::ElasticEaseIn(InTime, InPeriod);
	case EEaseType::ElasticOut:
		return FFairyEaseHelper::ElasticEaseOut(InTime, InPeriod);
	case EEaseType::ElasticInOut:
		return FFairyEaseHelper::ElasticEaseInOut(InTime, InPeriod);
	case EEaseType::BackIn:
		return FFairyEaseHelper::BackEaseIn(InTime);
	case EEaseType::BackOut:
		return FFairyEaseHelper::BackEaseOut(InTime);
	case EEaseType::BackInOut:
		return FFairyEaseHelper::BackEaseInOut(InTime);
	case EEaseType::BounceIn:
		return FFairyEaseHelper::BounceEaseIn(InTime);
	case EEaseType::BounceOut:
		return FFairyEaseHelper::BounceEaseOut(InTime);
	case EEaseType::BounceInOut:
		return FFairyEaseHelper::BounceEaseInOut(InTime);
	default:
		return FFairyEaseHelper::QuadEaseIn(InTime);
	}
}
