// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionItemPivot.h"
#include "FairyApplication.h"
#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"


FFairyTransitionItemPivot::FFairyTransitionItemPivot()
	: FFairyTransitionNoTweenItem(EFairyTransitionItemType::Pivot)
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

void FFairyTransitionItemPivot::NoTweenKeyFrameApply()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FFairyTransitionItemPivot::NoTweenKeyFrameApply(...);"));
	GetTarget()->SetPivot(startDataPtr->Pivot);
}
