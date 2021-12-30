#pragma once

#include "CoreMinimal.h"
#include "UI/Transition/FairyTransitionItemBase.h"


struct FRotationFrameData
{
    float rotate;
};

class FFairyTransitionItemRotation : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FRotationFrameData> startDataPtr;
	TSharedPtr<FRotationFrameData> endDataPtr;

public:
	FFairyTransitionItemRotation();
	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InTweener);
};