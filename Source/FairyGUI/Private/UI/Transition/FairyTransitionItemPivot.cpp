// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemPivot.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemPivot::FFairyTransitionItemPivot()
	: FFairyTransitionItemBase(EFairyTransitionItemType::Pivot)
{
}

void FFairyTransitionItemPivot::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);

	startDataPtr = MakeShareable(new FPivotFrameData);

	startDataPtr->bChangeAxisX = InBuffer->ReadBool();
	startDataPtr->bChangeAxisY = InBuffer->ReadBool();

	startDataPtr->Pivot.X = InBuffer->ReadFloat();
	startDataPtr->Pivot.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemPivot::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemPivot::RunItem();"));
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
	callback->GetDelegate().BindRaw(this, &FFairyTransitionItemPivot::EndCallback);
	OutTweenerList.Push(callback);
}

void FFairyTransitionItemPivot::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemPivot::EndCallback(...);"));
	GetTarget()->SetPivot(startDataPtr->Pivot);
}
