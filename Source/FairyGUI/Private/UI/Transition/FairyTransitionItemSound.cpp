// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemSound.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemSound::FFairyTransitionItemSound()
	: FFairyTransitionItemBase(EFairyTransitionItemType::Sound)
{
}

void FFairyTransitionItemSound::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	SoundUrl = InBuffer->ReadFNameFromCache();
	SoundVolume = InBuffer->ReadFloat();
}

void FFairyTransitionItemSound::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSound::RunItem();"));
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
	callback->GetDelegate().BindRaw(this, &FFairyTransitionItemSound::KeyFrameApply);
	OutTweenerList.Push(callback);
}

void FFairyTransitionItemSound::KeyFrameApply(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSound::KeyFrameApply(...);"));

	UFairyApplication::Get()->PlaySound(SoundUrl.ToString(), SoundVolume);
}
