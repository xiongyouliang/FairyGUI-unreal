// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemSound : public FFairyTransitionItemBase
{
private:
	FName SoundUrl;
	float SoundVolume;
public:
	FFairyTransitionItemSound();

	virtual void ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void KeyFrameApply(UFairyTweener* InFairyTweener);
};
