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
	if (_Target)
	{
		FVector2D newPos = startPos + (dstPos - startPos) * InTime;
		_Target->SetPosition(newPos);
	}
}

// Size tweener
bool UFairyTweenerSize::Init(float InDuration, FVector2D InStartSize, FVector2D InDstSize)
{
	UFairyTweenerInterval::Init(InDuration);

	startSize = InStartSize;
	dstSize = InDstSize;
	return true;
}

void UFairyTweenerSize::Update(float InTime)
{
	if (_Target)
	{
		FVector2D newSize = startSize + (dstSize - startSize) * InTime;
		_Target->SetSize(newSize);
	}
}

// Opacity tweener
bool UFairyTweenerOpacity::Init(float InDuration, float InStartOpacity, float InDstOpacity)
{
	UFairyTweenerInterval::Init(InDuration);

	startOpacity = InStartOpacity;
	dstOpacity = InDstOpacity;
	return true;
}

void UFairyTweenerOpacity::Update(float InTime)
{
	if (_Target)
	{
		float newOpacity = startOpacity + (dstOpacity - startOpacity) * InTime;
		_Target->SetAlpha(newOpacity);
	}
}

// Rotate tweener
bool UFairyTweenerRotate::Init(float InDuration, float InStartRotate, float InDstRotate)
{
	UFairyTweenerInterval::Init(InDuration);

	startRotate = InStartRotate;
	dstRotate = InDstRotate;
	return true;
}

void UFairyTweenerRotate::Update(float InTime)
{
	if (_Target)
	{
		float newRotate = startRotate + (dstRotate - startRotate) * InTime;
		_Target->SetRotation(newRotate);
	}
}

// Scale Tweener
bool UFairyTweenerScale::Init(float InDuration, FVector2D InStartScale, FVector2D InDstScale)
{
	UFairyTweenerInterval::Init(InDuration);

	startScale = InStartScale;
	dstScale = InDstScale;
	return true;
}

void UFairyTweenerScale::Update(float InTime)
{
	if (_Target)
	{
		FVector2D newScale = startScale + (dstScale - startScale) * InTime;
		_Target->SetScale(newScale);
	}
}

// Skew Tweener
bool UFairyTweenerSkew::Init(float InDuration, FVector2D InStartSkew, FVector2D InDstSkew)
{
	UFairyTweenerInterval::Init(InDuration);

	startSkew = InStartSkew;
	dstSkew = InDstSkew;
	return true;
}

void UFairyTweenerSkew::Update(float InTime)
{
	if (_Target)
	{
		FVector2D newSkew = startSkew + (dstSkew - startSkew) * InTime;
		_Target->SetSkew(newSkew);
	}
}

// Color Tweener

bool UFairyTweenerColor::Init(float InDuration, FColor InStartColor, FColor InDstColor)
{
	UFairyTweenerInterval::Init(InDuration);

	startColor = InStartColor;
	dstColor = InDstColor;
	return true;
}

void UFairyTweenerColor::Update(float InTime)
{
	if (_Target)
	{
		FColor newColor;
		newColor.R = startColor.R + (dstColor.R - startColor.R) * InTime;
		newColor.G = startColor.G + (dstColor.G - startColor.G) * InTime;
		newColor.B = startColor.B + (dstColor.B - startColor.B) * InTime;
		newColor.A = startColor.A + (dstColor.A - startColor.A) * InTime;
		_Target->SetColor(newColor);
	}
}
