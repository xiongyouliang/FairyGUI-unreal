// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FColorFilterFrameData {
	float luminance;
	float contrast;
	float sharpness;
	float hue;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemColorFilter : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FColorFilterFrameData> startDataPtr;
	TSharedPtr<FColorFilterFrameData> endDataPtr;
public:
	FFairyTransitionItemColorFilter();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
