#pragma once

#include "CoreMinimal.h"
#include "UI/Transition/FairyTransitionItemBase.h"

struct FAlphaFrameData
{
	float alpha;
};

class FFairyTransitionItemAlpha : public FFairyTransitionTweenableItem
{
private:
	TSharedPtr<FAlphaFrameData> startDataPtr;
	TSharedPtr<FAlphaFrameData> endDataPtr;

public:
	FFairyTransitionItemAlpha();
	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) override;
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) override;

	virtual void RunItem() override;

	void EndCallback(UFairyTweener* InTweener);
};
