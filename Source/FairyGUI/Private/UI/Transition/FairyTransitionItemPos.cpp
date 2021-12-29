#include "UI/Transition/FairyTransitionItemPos.h"
#include "FairyApplication.h"
#include "UI/FairyObject.h"
#include "Tween/FairyTweenerInterval.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"

FFairyTransitionItemPos::FFairyTransitionItemPos()
	: FFairyTransitionTweenableItem(EFairyTransitionItemType::XY)
	, bUseGuideLine(false)
{
}

void FFairyTransitionItemPos::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FPosFrameData());

	startDataPtr->bChangeAxisX = InBuffer->ReadBool();
	startDataPtr->bChangeAxisY = InBuffer->ReadBool();

	startDataPtr->Pos.X = InBuffer->ReadFloat();
	startDataPtr->Pos.Y = InBuffer->ReadFloat();

	if (InBuffer->Version >= 2)
	{
		startDataPtr->bUsePercent = InBuffer->ReadBool();
	}
}

void FFairyTransitionItemPos::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FPosFrameData());

	endDataPtr->bChangeAxisX = InBuffer->ReadBool();
	endDataPtr->bChangeAxisY = InBuffer->ReadBool();

	endDataPtr->Pos.X = InBuffer->ReadFloat();
	endDataPtr->Pos.Y = InBuffer->ReadFloat();

	if (InBuffer->Version >= 2)
	{
		endDataPtr->bUsePercent = InBuffer->ReadBool();
	}
}

void FFairyTransitionItemPos::ParsePathData(FairyGUI::FByteBuffer* InBuffer)
{
	// todo: parse path data
}

void FFairyTransitionItemPos::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	//UE_LOG(LogFairyGUI, Warning, TEXT("FairyTransitionItemPos::RunItem()"));
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	float delayTime = StartTime;
	if (InPreviousItem)
	{
		delayTime = StartTime - InPreviousItem->GetStartTime() - InPreviousItem->GetDuration();
	}

	if (IsHasTween())
	{
		if (delayTime > 0.0f)
		{
			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(delayTime);
			OutTweenerList.Push(delay);
		}

		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		UFairyTweenerPos* move = TweenMgr->CreateTweenerPos(config->Duration, startDataPtr->Pos, endDataPtr->Pos);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* ease = TweenMgr->CreateTweenerEase(move, config->EaseType, 0);
			OutTweenerList.Push(ease);
		}
		else
		{
			OutTweenerList.Push(move);
		}
	}
	else
	{
		if (delayTime > 0.0f)
		{
			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(delayTime);
			OutTweenerList.Push(delay);
		}
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemPos::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemPos::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FairyTransitionItemPos::EndCallback()"));
	if (!IsHasTween())
	{
		GetTarget()->SetPosition(startDataPtr->Pos);
	}
}