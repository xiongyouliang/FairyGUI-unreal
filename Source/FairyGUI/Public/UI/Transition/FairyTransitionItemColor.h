// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FColorFrameData {
	FColor Color;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemColor : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FColorFrameData> startDataPtr;
	TSharedPtr<FColorFrameData> endDataPtr;
public:
	FFairyTransitionItemColor();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
