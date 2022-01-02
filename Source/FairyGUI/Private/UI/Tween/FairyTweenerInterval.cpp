#include "UI/Tween/FairyTweenerInterval.h"

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

void UFairyTweenerInterval::SetTarget(UFairyObject* InTarget)
{
	UFairyTweenerFiniteTime::SetTarget(InTarget);
	bDone = false;
}

void UFairyTweenerInterval::Reset()
{
	bFirstTick = false;
	elapsedTime = K_MATH_EPSILON;
	bDone = false;
}

void UFairyTweenerInterval::Init(float InDuration)
{
	UFairyTweenerFiniteTime::Init(InDuration);

	elapsedTime = K_MATH_EPSILON;
	bFirstTick = true;
	bDone = false;
}

// Delay Tweener
bool UFairyTweenerDelay::Init(float InDuration)
{
	UFairyTweenerInterval::Init(InDuration);
	return true;
}

void UFairyTweenerDelay::Update(float InTime)
{
	return;
}

// Sequence Tweener
bool UFairyTweenerSequence::Init(const TArray<UFairyTweenerFiniteTime*>& InTweenerList)
{
	tweenerList = InTweenerList;
	float totalDuration = 0.0f;
	for (size_t i = 0; i < tweenerList.Num(); i++)
	{
		UFairyTweenerFiniteTime* element = tweenerList[i];
		totalDuration += element->GetDuration();
	}
	UFairyTweenerInterval::Init(totalDuration);

	curIndex = 0;

	return true;
}

void UFairyTweenerSequence::Step(float InDeltaTime)
{
	UFairyTweenerFiniteTime* curTweener = nullptr;
	UFairyTweenerFiniteTime* nextTweener = nullptr;
	// Sub-Tweener Step first
	if (curIndex < tweenerList.Num())
	{
		curTweener = tweenerList[curIndex];
		curTweener->Step(InDeltaTime);

		if (curTweener->IsDone())
		{
			curIndex++;
		}

		if (curIndex == tweenerList.Num())
		{
			bDone = true;
		}
	}
}

void UFairyTweenerSequence::SetTarget(UFairyObject* InTarget)
{
	Super::SetTarget(InTarget);
	for (size_t i = 0; i < tweenerList.Num(); i++)
	{
		UFairyTweenerFiniteTime* element = tweenerList[i];
		element->SetTarget(InTarget);
	}
}

void UFairyTweenerSequence::Reset()
{
	Super::Reset();
	for (size_t i = 0; i < tweenerList.Num(); i++)
	{
		UFairyTweenerFiniteTime* element = tweenerList[i];
		element->Reset();
	}
	curIndex = 0;
}

// Repeat tweener

bool UFairyTweenerRepeat::Init(UFairyTweenerFiniteTime* InTweener, uint32 repeatTimes)
{
	m_passedTimes = 0;
	m_repeatTimes = repeatTimes;
	_innerTweener = InTweener;

	SetDuration(InTweener->GetDuration());
	return true;
}

void UFairyTweenerRepeat::Step(float InDeltaTime)
{
	//Super::Step(InDeltaTime);
	_innerTweener->Step(InDeltaTime);

	if (_innerTweener->IsDone() && m_passedTimes < m_repeatTimes)
	{
		m_passedTimes++;
		_innerTweener->Reset();
	}

	if (m_passedTimes == m_repeatTimes)
	{
		bDone = true;
	}
}

void UFairyTweenerRepeat::SetTarget(UFairyObject* InTarget)
{
	Super::SetTarget(InTarget);

	if (_innerTweener)
	{
		_innerTweener->SetTarget(InTarget);
	}
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

bool UFairyTweenerShake::Init(float InDuration, float InAmplitude)
{
	Super::Init(InDuration);

	Amplitude = InAmplitude;
	Direction = FMath::RandRange(0, 3);

	return true;
}

void UFairyTweenerShake::SetTarget(UFairyObject* InTarget)
{
	Super::SetTarget(InTarget);

	if (InTarget)
	{
		originPos = InTarget->GetPosition();
	}
}

void UFairyTweenerShake::Step(float InDeltaTime)
{
	Super::Step(InDeltaTime);
	if (IsDone())
	{
		GetTarget()->SetPosition(originPos);
	}
	else
	{
		/**
		* This is simple shake calculation, but it is run fast.
		* Add some random to direction or/and amplitude value will has a good effect.
		*/
		FVector2D newPos = originPos;
		if (Direction == 0)
		{
			newPos.X -= Amplitude;
		}

		if (Direction == 1)
		{
			newPos.Y -= Amplitude;
		}

		if (Direction == 2)
		{
			newPos.X += Amplitude;
		}

		if (Direction == 3)
		{
			newPos.Y += Amplitude;
		}
		GetTarget()->SetPosition(newPos);

		//Direction++;
		//Direction %= 4;
		Direction = FMath::RandRange(0, 3);
	}
}
