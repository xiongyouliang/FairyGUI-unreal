// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FSkewFrameData {
	bool bChangeWidth;
	bool bChangeHeight;
	FVector2D SkewValue;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemSkew : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FSkewFrameData> startDataPtr;
	TSharedPtr<FSkewFrameData> endDataPtr;
public:
	FFairyTransitionItemSkew();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
