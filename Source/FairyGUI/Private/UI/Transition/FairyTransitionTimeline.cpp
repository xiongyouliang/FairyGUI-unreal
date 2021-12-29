// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionTimeline.h"
#include "UI/Transition/FairyTransition.h"
#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/FairyObject.h"

#include "Tween/FairyTweenMgr.h"

#include "FairyApplication.h"

class UFairyTweenerFiniteTime;
class UFairyTweenerSequence;

void FFairyTransitionTimeline::SetTarget(UFairyObject* InTargetObject)
{
	TargetObjectPtr = InTargetObject;
}

UFairyObject* FFairyTransitionTimeline::GetTarget()
{
	if (TargetObjectPtr.IsValid())
	{
		return TargetObjectPtr.Get();
	}
	return nullptr;
}

void FFairyTransitionTimeline::SetOwner(UFairyTransition* InOwnerObject)
{
	OwnerObjectPtr = InOwnerObject;
}

UFairyTransition* FFairyTransitionTimeline::GetOwner()
{
	if (OwnerObjectPtr.IsValid())
	{
		return OwnerObjectPtr.Get();
	}
	return nullptr;
}

void FFairyTransitionTimeline::SetTimelineType(EFairyTransitionItemType InTimelineType)
{
	TimelineType = InTimelineType;
}

EFairyTransitionItemType FFairyTransitionTimeline::GetTimelineType()
{
	return TimelineType;
}

uint32 FFairyTransitionTimeline::GetRepeatTimes()
{
	// todo: get timeline repeat times from UFairyTransition object;
	return 0;
}

void FFairyTransitionTimeline::AddTransitionItem(FFairyTransitionItemBase* InTransitionItem)
{
	TSharedPtr<FFairyTransitionItemBase> item = MakeShareable(InTransitionItem);
	if (!ItemList.Contains(item))
	{
		ItemList.Push(item);
	}
}

void FFairyTransitionTimeline::Play()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionTimeline::Play()"));

	TArray<UFairyTweenerFiniteTime*> tweenerList;

	FFairyTransitionItemBase* previousItem = nullptr;
	for (size_t i = 0; i < ItemList.Num(); i++)
	{
		TSharedPtr<FFairyTransitionItemBase> item = ItemList[i];
		if (item.IsValid())
		{
			item->ConstructTweenerList(tweenerList, previousItem);
			previousItem = item.Get();
		}
	}

	UFairyObject* TargetObjet = previousItem->GetTarget();
	UFairyTweenerFiniteTime* finalTweener = nullptr;
	if (tweenerList.Num() > 1)
	{
		UFairyTweenerSequence* Sequence = UFairyApplication::Get()->GetTweenMgr()->CreateTweenerSequence(tweenerList);
		finalTweener = Sequence;
		
	}

	if (tweenerList.Num() == 1)
	{
		finalTweener = tweenerList[0];
	}

	if (finalTweener && TargetObjet)
	{
		int32 repeatTime = GetOwner()->GetRepeatTimes();
		if (repeatTime < 0)
		{
			// todo: repeat forever
		}
		else if (repeatTime > 1)
		{
			UFairyTweenerRepeat* repeatTweener = UFairyApplication::Get()->GetTweenMgr()->CreateTweenerRepeat(finalTweener, repeatTime);
			TargetObjet->RunTween(repeatTweener);
		}
		else
		{
			TargetObjet->RunTween(finalTweener);
		}
	}
}

