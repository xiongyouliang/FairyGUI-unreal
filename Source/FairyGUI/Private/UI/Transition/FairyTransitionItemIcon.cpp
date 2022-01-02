// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemIcon.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemIcon::FFairyTransitionItemIcon()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Icon)
{
}

void FFairyTransitionItemIcon::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	IconUrl = InBuffer->ReadFNameFromCache();
}

void FFairyTransitionItemIcon::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemIcon::NoTweenKeyFrameApply(...);"));
	if (!IconUrl.IsNone())
	{
		GetTarget()->SetIcon(IconUrl.ToString());
	}
}
