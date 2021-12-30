// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemVisible.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemVisible::FFairyTransitionItemVisible()
	: FFairyTransitionItemBase(EFairyTransitionItemType::Visible)
{
}

void FFairyTransitionItemVisible::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	bVisible = InBuffer->ReadBool();
}

void FFairyTransitionItemVisible::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemVisible::RunItem();"));
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

	UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
	callback->GetDelegate().BindRaw(this, &FFairyTransitionItemVisible::KeyFrameApply);
	OutTweenerList.Push(callback);
}

void FFairyTransitionItemVisible::KeyFrameApply(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemVisible::KeyFrameApply(...);"));
	GetTarget()->SetVisible(bVisible);
}
