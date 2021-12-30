// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemSkew.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemSkew::FFairyTransitionItemSkew()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::Skew)
{
}

void FFairyTransitionItemSkew::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FSkewFrameData);

	startDataPtr->bChangeWidth = InBuffer->ReadBool();
	startDataPtr->bChangeHeight = InBuffer->ReadBool();

	startDataPtr->SkewValue.X = InBuffer->ReadFloat();
	startDataPtr->SkewValue.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemSkew::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FSkewFrameData);

	endDataPtr->bChangeWidth = InBuffer->ReadBool();
	endDataPtr->bChangeHeight = InBuffer->ReadBool();

	endDataPtr->SkewValue.X = InBuffer->ReadFloat();
	endDataPtr->SkewValue.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemSkew::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSkew::RunItem();"));
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	float delayTime = StartTime;
	if (InPreviousItem)
	{
		delayTime = StartTime - InPreviousItem->GetStartTime() - InPreviousItem->GetDuration();
	}

	if (delayTime > 0.0f)
	{
		UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(delayTime);
		OutTweenerList.Push(delay);
	}


	if (IsHasTween())
	{
		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		UFairyTweenerSkew* SkewTweener = TweenMgr->CreateTweenerSkew(config->Duration, startDataPtr->SkewValue, endDataPtr->SkewValue);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(SkewTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(SkewTweener);
		}
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemSkew::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemSkew::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSkew::EndCallback(...);"));
	if (!IsHasTween())
	{
		GetTarget()->SetSkew(startDataPtr->SkewValue);
	}
}
