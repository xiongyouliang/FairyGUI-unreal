// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FScaleFrameData {
	FVector2D Scale;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemScale : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FScaleFrameData> startDataPtr;
	TSharedPtr<FScaleFrameData> endDataPtr;
public:
	FFairyTransitionItemScale();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
