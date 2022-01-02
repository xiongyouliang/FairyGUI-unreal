#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FPosFrameData {
	bool bChangeAxisX;
	bool bChangeAxisY;
	bool bUsePercent;
	FVector2D Pos;
};

class FFairyTransitionItemPos : public FFairyTransitionTweenableItem
{
private:
	bool bUseGuideLine;
	TSharedPtr<FPosFrameData> startDataPtr;
	TSharedPtr<FPosFrameData> endDataPtr;

public:
	FFairyTransitionItemPos();

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParsePathData(FairyGUI::FByteBuffer* InBufffer) override;

	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) override;

	void EndCallback(UFairyTweener* InFairyTweener);
};
