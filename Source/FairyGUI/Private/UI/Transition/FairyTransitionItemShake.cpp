// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemShake.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"

class UFairyTweenerShake;

FFairyTransitionItemShake::FFairyTransitionItemShake()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Shake)
{
}

void FFairyTransitionItemShake::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);

	Amplitude = InBuffer->ReadFloat();
	Duration = InBuffer->ReadFloat();
}

void FFairyTransitionItemShake::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemShake::NoTweenKeyFrameApply(...);"));

	// UFairyApplication::Get()->PlaySound(SoundUrl.ToString(), SoundVolume);

	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();
	UFairyTweenerShake* shakeTweener = TweenMgr->CreateTweenerShake(Duration, Amplitude);

	GetTarget()->RunTween(shakeTweener);
}
