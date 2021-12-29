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

void FFairyTransitionItemSize::RunItem()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSize::RunItem();"));
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	if (IsHasTween())
	{
		TArray<UFairyTweenerFiniteTime*> tweenerList;

		if (StartTime > 0.0f)
		{
			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);
		}

		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		UFairyTweenerInterval* finalTweener;
		UFairyTweenerSize* SizeTweener = TweenMgr->CreateTweenerSize(config->Duration, startDataPtr->Size, endDataPtr->Size);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(SizeTweener, config->EaseType, 0);
			tweenerList.Push(Ease);
			finalTweener = Ease;
		}
		else
		{
			tweenerList.Push(SizeTweener);
			finalTweener = SizeTweener;
		}

		if (tweenerList.Num() > 1)
		{
			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);
			finalTweener = sequence;
		}
		
		if (config->Repeat > 0)
		{
			finalTweener = TweenMgr->CreateTweenerRepeat(finalTweener, config->Repeat);
		}

		GetTarget()->RunTween(finalTweener);
	}
	else
	{
		if (StartTime)
		{
			TArray<UFairyTweenerFiniteTime*> tweenerList;

			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);

			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemSize::EndCallback);
			tweenerList.Push(callback);

			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);

			GetTarget()->RunTween(sequence);
		}
		else
		{
			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemSize::EndCallback);

			GetTarget()->RunTween(callback);
		}
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
