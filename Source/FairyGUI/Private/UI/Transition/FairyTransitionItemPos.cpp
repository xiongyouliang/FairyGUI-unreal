#include "UI/Transition/FairyTransitionItemPos.h"

FFairyTransitionItemPos::FFairyTransitionItemPos()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::XY)
	, bUseGuideLine(false)
{
}

void FFairyTransitionItemPos::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FPosFrameData());
	endDataPtr = MakeShareable(new FPosFrameData());
}

void FFairyTransitionItemPos::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FPosFrameData());

	startDataPtr->bChangeAxisX = InBuffer->ReadBool();
	startDataPtr->bChangeAxisY = InBuffer->ReadBool();

	startDataPtr->Pos.X = InBuffer->ReadFloat();
	startDataPtr->Pos.Y = InBuffer->ReadFloat();
}

void FFairyTransitionItemPos::ParsePathData(FairyGUI::FByteBuffer* InBuffer)
{

}

void FFairyTransitionItemPos::RunItem()
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FairyTransitionItemPos::RunItem()"));
}