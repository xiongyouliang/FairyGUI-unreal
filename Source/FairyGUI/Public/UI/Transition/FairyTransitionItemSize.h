// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FSizeFrameData {
	bool bChangeWidth;
	bool bChangeHeight;
	FVector2D Size;
};

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemSize : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FSizeFrameData> startDataPtr;
	TSharedPtr<FSizeFrameData> endDataPtr;
public:
	FFairyTransitionItemSize();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
