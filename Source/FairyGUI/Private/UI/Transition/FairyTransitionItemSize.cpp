// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemSize.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemSize::FFairyTransitionItemSize()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::Size)
{
}

void FFairyTransitionItemSize::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FSizeFrameData);

	startDataPtr->bChangeWidth = InBuffer->ReadBool();
	startDataPtr->bChangeHeight = InBuffer->ReadBool();

	startDataPtr->Size.X = InBuffer->ReadFloat();
	startDataPtr->Size.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemSize::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FSizeFrameData);

	endDataPtr->bChangeWidth = InBuffer->ReadBool();
	endDataPtr->bChangeHeight = InBuffer->ReadBool();

	endDataPtr->Size.X = InBuffer->ReadFloat();
	endDataPtr->Size.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemSize::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSize::RunItem();"));
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
		UFairyTweenerSize* SizeTweener = TweenMgr->CreateTweenerSize(config->Duration, startDataPtr->Size, endDataPtr->Size);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(SizeTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(SizeTweener);
		}
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemSize::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemSize::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSize::EndCallback(...);"));
	if (!IsHasTween())
	{
		GetTarget()->SetSize(startDataPtr->Size);
	}
}
