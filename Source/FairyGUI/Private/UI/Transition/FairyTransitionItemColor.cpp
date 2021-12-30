// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemColor.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemColor::FFairyTransitionItemColor()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::Color)
{
}

void FFairyTransitionItemColor::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FColorFrameData);

	startDataPtr->Color = InBuffer->ReadColor();
}

void FFairyTransitionItemColor::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FColorFrameData);

	endDataPtr->Color = InBuffer->ReadColor();
}

void FFairyTransitionItemColor::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemColor::RunItem();"));
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
		UFairyTweenerColor* ColorTweener = TweenMgr->CreateTweenerColor(config->Duration, startDataPtr->Color, endDataPtr->Color);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(ColorTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(ColorTweener);
		}
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemColor::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemColor::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemColor::EndCallback(...);"));
	if (!IsHasTween())
	{
		GetTarget()->SetColor(startDataPtr->Color);
	}
}
