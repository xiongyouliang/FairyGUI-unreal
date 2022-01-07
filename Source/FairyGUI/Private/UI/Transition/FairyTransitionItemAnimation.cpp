// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemAnimation.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"
#include "UI/FairyMovieClip.h"


FFairyTransitionItemAnimation::FFairyTransitionItemAnimation()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Animation)
{
}

void FFairyTransitionItemAnimation::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	bPlaying = InBuffer->ReadBool();
	curFrame = InBuffer->ReadInt();
}

void FFairyTransitionItemAnimation::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemAnimation::NoTweenKeyFrameApply(...);"));
	UFairyObject* TargetObject = GetTarget();
	if (TargetObject->IsA(UFairyMovieClip::StaticClass()))
	{
		UFairyMovieClip* Animation = (UFairyMovieClip*)TargetObject;
		
		Animation->SetFrame(curFrame);

		if (Animation->IsPlaying() != bPlaying)
		{
			Animation->SetPlaying(bPlaying);
		}
		
	}
}
