// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

/**
 * 
 */
class FAIRYGUI_API FFairyTransitionItemShake : public FFairyTransitionNoTweenItem
{
private:
	float Amplitude;
	float Duration;
public:
	FFairyTransitionItemShake();

	virtual void ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;

	virtual void NoTweenKeyFrameApply() override;
};
