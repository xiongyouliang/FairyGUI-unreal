// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionTimeline.h"
#include "UI/Transition/FairyTransition.h"
#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/FairyObject.h"

#include "UI/Tween/FairyTweenMgr.h"

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

int32 FFairyTransitionTimeline::GetRepeatTimes()
{
	/**
	* Note:
	* In current FairyGUI Editor Version(2021.3.1), the repeat times come from two source:
	* 1. The transition autoplay setting
	* 2. The keyframe with tween property setting.
	* 
	* The first source with effect all timeline in same transition.
	* The second source just property in one TweenItem.
	* When a timeline has more then one tween, just use first tween's repeat setting.
	* If the timeline has no tween, use transition's autoplay setting.
	*/
	int32 repeatTimes = 0;
	for (size_t i = 0; i < ItemList.Num(); i++)
	{
		TSharedPtr<FFairyTransitionItemBase> ItemPtr = ItemList[i];
		if (ItemPtr->IsHasTween())
		{
			return ItemPtr->GetRepeatTimes();
		}
	}

	return GetOwner()->GetRepeatTimes();
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
		int32 repeatTime = GetRepeatTimes();
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

