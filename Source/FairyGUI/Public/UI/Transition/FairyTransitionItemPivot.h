// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FPivotFrameData {
	bool bChangeAxisX;
	bool bChangeAxisY;
	FVector2D Pivot;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemPivot : public FFairyTransitionItemBase
{
private:
	TSharedPtr<FPivotFrameData> startDataPtr;
public:
	FFairyTransitionItemPivot();

	virtual void ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
