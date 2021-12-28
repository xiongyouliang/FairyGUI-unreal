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

void FFairyTransitionItemPos::RunItem()
{
	//UE_LOG(LogFairyGUI, Warning, TEXT("FairyTransitionItemPos::RunItem()"));
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	if (IsHasTween())
	{
		TArray<UFairyTweenerFiniteTime*> tweenerList;

		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		if (StartTime > 0.0f)
		{
			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);
		}
		
		UFairyTweenerInterval* finalTween;
		UFairyTweenerPos* move = TweenMgr->CreateTweenerPos(config->Duration, startDataPtr->Pos, endDataPtr->Pos);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* ease = TweenMgr->CreateTweenerEase(move, config->EaseType, 0);
			tweenerList.Push(ease);
			finalTween = ease;
		}
		else
		{
			tweenerList.Push(move);
			finalTween = move;
		}

		if (tweenerList.Num() > 1)
		{
			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);
			finalTween = sequence;
		}

		if (config->Repeat > 0)
		{
			finalTween = TweenMgr->CreateTweenerRepeat(finalTween, config->Repeat);
		}

		GetTarget()->RunTween(finalTween);
	}
	else
	{
		if (StartTime)
		{
			TArray<UFairyTweenerFiniteTime*> tweenerList;

			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);

			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemPos::EndCallback);
			tweenerList.Push(callback);

			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);

			GetTarget()->RunTween(sequence);
		}
		else
		{
			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemPos::EndCallback);

			GetTarget()->RunTween(callback);
		}
	}
}

void FFairyTransitionItemPos::EndCallback(UFairyTweener* InFairyTweener)
{
	UE_LOG(LogFairyGUI, Warning, TEXT("FairyTransitionItemPos::EndCallback()"));
	if (!IsHasTween())
	{
		GetTarget()->SetPosition(startDataPtr->Pos);
	}
	else
	{
		// No action for tween
	}
}