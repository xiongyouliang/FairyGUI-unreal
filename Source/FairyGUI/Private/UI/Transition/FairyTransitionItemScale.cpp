// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemScale.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemScale::FFairyTransitionItemScale()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::Scale)
{
}

void FFairyTransitionItemScale::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FScaleFrameData);

	startDataPtr->Scale.X = InBuffer->ReadFloat();
	startDataPtr->Scale.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemScale::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FScaleFrameData);

	endDataPtr->Scale.X = InBuffer->ReadFloat();
	endDataPtr->Scale.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemScale::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemScale::RunItem();"));
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
		UFairyTweenerScale* ScaleTweener = TweenMgr->CreateTweenerScale(config->Duration, startDataPtr->Scale, endDataPtr->Scale);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(ScaleTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(ScaleTweener);
		}
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemScale::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemScale::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemScale::EndCallback(...);"));
	if (!IsHasTween())
	{
		GetTarget()->SetScale(startDataPtr->Scale);
	}
}
