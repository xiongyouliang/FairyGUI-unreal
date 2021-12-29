#include "UI/Transition/FairyTransitionItemAlpha.h"
#include "FairyApplication.h"
#include "UI/FairyObject.h"
#include "Tween/FairyTweenerInterval.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"

FFairyTransitionItemAlpha::FFairyTransitionItemAlpha()
	:FFairyTransitionTweenableItem(EFairyTransitionItemType::Alpha)
{

}

void FFairyTransitionItemAlpha::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FAlphaFrameData);

	startDataPtr->alpha = InBuffer->ReadFloat();
}

void FFairyTransitionItemAlpha::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FAlphaFrameData);

	endDataPtr->alpha = InBuffer->ReadFloat();
}

void FFairyTransitionItemAlpha::RunItem()
{
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	if (IsHasTween())
	{
		TArray<UFairyTweenerFiniteTime*> tweenerList;

		if (StartTime > 0.0f)
		{
			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);
		}

		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		UFairyTweenerInterval* finalTweener;
		UFairyTweenerOpacity* AlphaTweener = TweenMgr->CreateTweenerOpacity(config->Duration, startDataPtr->alpha, endDataPtr->alpha);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(AlphaTweener, config->EaseType, 0);
			tweenerList.Push(Ease);
			finalTweener = Ease;
		}
		else
		{
			tweenerList.Push(AlphaTweener);
			finalTweener = AlphaTweener;
		}

		if (tweenerList.Num() > 1)
		{
			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);
			finalTweener = sequence;
		}

		if (config->Repeat > 0)
		{
			finalTweener = TweenMgr->CreateTweenerRepeat(finalTweener, config->Repeat);
		}

		GetTarget()->RunTween(finalTweener);
	}
	else
	{
		if (StartTime)
		{
			TArray<UFairyTweenerFiniteTime*> tweenerList;

			UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(StartTime);
			tweenerList.Push(delay);

			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemAlpha::EndCallback);
			tweenerList.Push(callback);

			UFairyTweenerSequence* sequence = TweenMgr->CreateTweenerSequence(tweenerList);

			GetTarget()->RunTween(sequence);
		}
		else
		{
			UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
			callback->GetDelegate().BindRaw(this, &FFairyTransitionItemAlpha::EndCallback);

			GetTarget()->RunTween(callback);
		}
	}
}

void FFairyTransitionItemAlpha::EndCallback(UFairyTweener* InTweener)
{
	if (!IsHasTween())
	{
		GetTarget()->SetAlpha(startDataPtr->alpha);
	}
}

