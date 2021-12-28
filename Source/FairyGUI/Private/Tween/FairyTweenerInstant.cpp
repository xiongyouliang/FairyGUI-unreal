#include "Tween/FairyTweenerInstant.h"


bool UFairyTweenerInstant::IsInstant()
{
	return true;
}

void UFairyTweenerInstant::SetTarget(UFairyObject* InTarget)
{
	UFairyTweenerFiniteTime::SetTarget(InTarget);
	bDone = false;
}

bool UFairyTweenerInstant::IsDone() const
{
	return bDone;
}

void UFairyTweenerInstant::Step(float InDeltaTime)
{
	float updateDt = 1.0f;
	Update(updateDt);
}

void UFairyTweenerInstant::Update(float InTime)
{
	bDone = true;
}

// Show Tweener
void UFairyTweenerShow::Update(float InTime)
{
	Super::Update(InTime);
	if (_Target)
	{
		_Target->SetVisible(true);
	}
}

// Hide Tweener
void UFairyTweenerHide::Update(float InTime)
{
	Super::Update(InTime);
	if (_Target)
	{
		_Target->SetVisible(false);
	}
}

// Change Pivot Tweener
void UFairyTweenerPivot::Update(float InTime)
{
	Super::Update(InTime);
	if (_Target)
	{
		_Target->SetPivot(newPivot);
	}
}

void UFairyTweenerPivot::Init(FVector2D InNewPivot)
{
	newPivot = InNewPivot;
}

// CallFunc Tweener
void UFairyTweenerCallFunc::Update(float InTime)
{
	Super::Update(InTime);
	if (_Target)
	{
		_delegate.ExecuteIfBound(this);
	}
}

bool UFairyTweenerCallFunc::Init()
{
	return true;
}

FTweenDelegate& UFairyTweenerCallFunc::GetDelegate()
{
	return _delegate;
}

// Dynamic CallFunc Tweener;
void UFairyTweenerDynamicCallFunc::Update(float InTime)
{
	Super::Update(InTime);
	if (_Target)
	{
		_delegate.ExecuteIfBound(this);
	}
}

bool UFairyTweenerDynamicCallFunc::Init(const FTweenDynamicDelegate& InDelegate)
{
	_delegate = InDelegate;
	return true;
}
