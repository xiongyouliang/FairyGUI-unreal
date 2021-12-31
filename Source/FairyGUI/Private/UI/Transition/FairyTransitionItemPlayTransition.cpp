// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemPlayTransition.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemPlayTransition::FFairyTransitionItemPlayTransition()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Transition)
{
}

void FFairyTransitionItemPlayTransition::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	TransitionName = InBuffer->ReadFNameFromCache();
	PlayTimes = InBuffer->ReadInt();
}

void FFairyTransitionItemPlayTransition::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemPlayTransition::NoTweenKeyFrameApply(...); TransitionName:%s"), *TransitionName.ToString());
	// todo: play transition which name is TransitionName
}
