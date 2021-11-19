#include "Tween/FairyTweenerInstant.h"

void UFairyTweenerInstant::StartWithTarget(UFairyObject* InTarget)
{
	UFairyTweenerFiniteTime::StartWithTarget(InTarget);
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
