// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemText.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemText::FFairyTransitionItemText()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Text)
{
}

void FFairyTransitionItemText::ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos)
{
	InBuffer->Seek(curPos, 2);
	TextStr = InBuffer->ReadStringFromCache();
}

void FFairyTransitionItemText::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemText::NoTweenKeyFrameApply(...);"));
	if (!TextStr.IsEmpty())
	{
		GetTarget()->SetText(TextStr);
	}
}
