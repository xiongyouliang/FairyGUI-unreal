#include "UI/Transition/FairyTransitionItemRotation.h"
#include "FairyApplication.h"
#include "UI/FairyObject.h"
#include "Tween/FairyTweenerInterval.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"

FFairyTransitionItemRotation::FFairyTransitionItemRotation()
	:FFairyTransitionTweenableItem(EFairyTransitionItemType::Rotation)
{

}

void FFairyTransitionItemRotation::ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer)
{
	startDataPtr = MakeShareable(new FRotationFrameData);

	startDataPtr->rotate = InBuffer->ReadFloat();
}

void FFairyTransitionItemRotation::ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer)
{
	endDataPtr = MakeShareable(new FRotationFrameData);

	endDataPtr->rotate = InBuffer->ReadFloat();
}

void FFairyTransitionItemRotation::ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem)
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
		UFairyTweenerRotate* RotationTweener = TweenMgr->CreateTweenerRotate(config->Duration, startDataPtr->rotate, endDataPtr->rotate);
		if (config->EaseType != EFairyEaseType::Linear)
		{
			UFairyTweenerEase* Ease = TweenMgr->CreateTweenerEase(RotationTweener, config->EaseType, 0);
			OutTweenerList.Push(Ease);
		}
		else
		{
			OutTweenerList.Push(RotationTweener);
		}
	}

	UFairyTweenerCallFunc* callback = TweenMgr->CreateTweenerCallFunc();
	callback->GetDelegate().BindRaw(this, &FFairyTransitionItemRotation::EndCallback);
	OutTweenerList.Push(callback);
}

void FFairyTransitionItemRotation::EndCallback(UFairyTweener* inTweener)
{
	if (IsHasTween())
	{
		//GetTarget()->SetRotation(endDataPtr->rotate);
	}
	else
	{
		GetTarget()->SetRotation(startDataPtr->rotate);
	}
}

