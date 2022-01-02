#include "UI/Transition/FairyTransitionItemAlpha.h"
#include "FairyApplication.h"
#include "UI/FairyObject.h"
#include "UI/Tween/FairyTweenerInterval.h"
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

void FFairyTransitionItemAlpha::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
{
	UFairyTweenMgr* TweenMgr = UFairyApplication::Get()->GetTweenMgr();

	float delayTime = StartTime;
	if (InPreviousItem)
	{
		delayTime = StartTime - InPreviousItem->GetStartTime() - InPreviousItem->GetDuration();
	}

	if (delayTime > 0.0f)
	{
		UFairyTweenerDelay* delay = TweenMgr->CreateTweenerDelay(delayTime);
		OutTweenerList.Push(delay);
	}

	if (IsHasTween())
	{
		FFariyTransitionTweenConfig* config = TweenConfigPtr.Get();
		UFairyTweenerOpacity* AlphaTweener = TweenMgr->CreateTweenerOpacity(config->Duration, startDataPtr->alpha, endDataPtr->alpha);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(AlphaTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(AlphaTweener);
		}
	}
	else
	{
		UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
		callback->GetDelegate().BindRaw(this, &FFairyTransitionItemAlpha::EndCallback);
		OutTweenerList.Push(callback);
	}
}

void FFairyTransitionItemAlpha::EndCallback(UFairyTweener* InTweener)
{
	if (!IsHasTween())
	{
		GetTarget()->SetAlpha(startDataPtr->alpha);
	}
}

