// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemSound.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemSound::FFairyTransitionItemSound()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Sound)
{
}

void FFairyTransitionItemSound::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	SoundUrl = InBuffer->ReadFNameFromCache();
	SoundVolume = InBuffer->ReadFloat();
}

void FFairyTransitionItemSound::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemSound::NoTweenKeyFrameApply(...);"));

	UFairyApplication::Get()->PlaySound(SoundUrl.ToString(), SoundVolume);
}
