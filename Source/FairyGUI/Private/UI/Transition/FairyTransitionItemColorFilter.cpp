// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemColorFilter.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemColorFilter::FFairyTransitionItemColorFilter()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::ColorFilter)
{
}

void FFairyTransitionItemColorFilter::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FColorFilterFrameData);

	startDataPtr->luminance = InBuffer->ReadFloat();
	startDataPtr->contrast = InBuffer->ReadFloat();
	startDataPtr->sharpness = InBuffer->ReadFloat();
	startDataPtr->hue = InBuffer->ReadFloat();
}

void FFairyTransitionItemColorFilter::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FColorFilterFrameData);

	endDataPtr->luminance = InBuffer->ReadFloat();
	endDataPtr->contrast = InBuffer->ReadFloat();
	endDataPtr->sharpness = InBuffer->ReadFloat();
	endDataPtr->hue = InBuffer->ReadFloat();
}

void FFairyTransitionItemColorFilter::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemColorFilter::RunItem();"));
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
		// todo: right now, there no color filter operate api for fairy object.
		
		// FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		// UFairyTweenerColor* ColorTweener = TweenMgr->CreateTweenerColor(config->Duration, startDataPtr->Color, endDataPtr->Color);
		// if (config->EaseType != EFairyEaseType::Linear)
		// {
		// 	UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(ColorTweener, config->EaseType, 0);
		// 	OutTweenerList.Push(Ease);
		// }
		// else
		// {
		// 	OutTweenerList.Push(ColorTweener);
		// }
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemColorFilter::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemColorFilter::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemColorFilter::EndCallback(...);"));
	if (!IsHasTween())
	{
	}
}
