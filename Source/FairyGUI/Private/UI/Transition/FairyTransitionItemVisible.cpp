// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemVisible.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemVisible::FFairyTransitionItemVisible()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Visible)
{
}

void FFairyTransitionItemVisible::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	bVisible = InBuffer->ReadBool();
}

void FFairyTransitionItemVisible::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemVisible::NoTweenKeyFrameApply(...);"));
	GetTarget()->SetVisible(bVisible);
}
