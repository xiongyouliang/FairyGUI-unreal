#include "UI/Transition/FairyTransition.h"
#include "UI/FairyComponent.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"
#include "Tween/GPath.h"
#include "FairyApplication.h"

const int32 OPTION_IGNORE_DISPLAY_CONTROLLER = 1;
const int32 OPTION_AUTO_STOP_DISABLED = 2;
const int32 OPTION_AUTO_STOP_AT_END = 4;


UFairyTransition::UFairyTransition() :
	bPlaying(false),
	bPaused(false),
	bAutoPlay(false),
	bReversed(false),
	TotalTimes(0),
	TotalTasks(0),
	Options(0),
	AutoPlayTimes(0),
	TotalDuration(0.0f),
	AutoPlayDelay(0.0f),
	TimeScale(1.0f),
	StartTime(0),
	EndTime(0)
{

}

UFairyTransition::~UFairyTransition()
{

}

FName UFairyTransition::GetName()
{
	return Name;
}

void UFairyTransition::ChangePlayTimes(int32 InTimes)
{
	TotalTimes = InTimes;
}

void UFairyTransition::SetAutoPlay(bool bInAutoPlay, int32 InTimes, float InDelay)
{
}

void UFairyTransition::Stop(bool bSetToComplete, bool bProcessCallback)
{
}

void UFairyTransition::StopItem(FFairyTransitionItemBase* item, bool bSetToComplete)
{
}

void UFairyTransition::SetPaused(bool bInPaused)
{

}

void UFairyTransition::SetValue(const FString& InLabel, const TArray<FNVariant>& InValues)
{
}

void UFairyTransition::SetHook(const FString& InLabel, FSimpleDelegate Callback)
{

}

void UFairyTransition::ClearHooks()
{
}

void UFairyTransition::SetTarget(const FString& InLabel, UFairyObject* InTarget)
{

}

void UFairyTransition::SetDuration(const FString& InLabel, float InDuration)
{
}

float UFairyTransition::GetLabelTime(const FString& InLabel) const
{
	return 0.0f;
}

void UFairyTransition::SetTimeScale(float InTimeScale)
{
}

void UFairyTransition::UpdateFromRelations(const FString& TargetID, const FVector2D& Delta)
{
}

void UFairyTransition::OnOwnerAddedToStage()
{
	if (bAutoPlay && !bPlaying)
	{
		Play(AutoPlayTimes, AutoPlayDelay);
	}
}

void UFairyTransition::OnOwnerRemovedFromStage()
{
	if ((Options & OPTION_AUTO_STOP_DISABLED) == 0)
	{
		Stop((Options & OPTION_AUTO_STOP_AT_END) != 0 ? true : false, false);
	}
}

void UFairyTransition::Play(int32 InTimes, float InDelay, float InStartTime, float InEndTime, bool bInReverse, FSimpleDelegate InCompleteCallback)
{
	Stop(true, true);

	TotalTimes = InTimes;
	StartTime = InStartTime;
	EndTime = InEndTime;

	bReversed = bInReverse;
	bPlaying = true;
	bPaused = false;
	CompleteCallback = InCompleteCallback;

	if (InDelay == 0)
	{
		OnDelayedPlay();
	}
	else
	{
		//DelayHandle = FFairyTweenHelper::DelayedCall(InDelay)->OnComplete(FSimpleDelegate::CreateUObject(this, &UFairyTransition::OnDelayedPlay))->GetHandle();
	}
}

void UFairyTransition::OnDelayedPlay()
{
}

void UFairyTransition::InternalPlay()
{
}

void UFairyTransition::PlayItem(FFairyTransitionItemBase* item)
{
}

void UFairyTransition::SkipAnimations()
{
}

void UFairyTransition::OnDelayedPlayItem(UFairyTweener* Tweener)
{
}

void UFairyTransition::OnTweenStart(UFairyTweener* Tweener)
{
}

void UFairyTransition::OnTweenUpdate(UFairyTweener* Tweener)
{

}

void UFairyTransition::OnTweenComplete(UFairyTweener* Tweener)
{
}

void UFairyTransition::OnPlayTransCompleted(FFairyTransitionItemBase* item)
{
	TotalTasks--;

	CheckAllComplete();
}

void UFairyTransition::CallHook(FFairyTransitionItemBase* item, bool bTweenEnd)
{
}

void UFairyTransition::CheckAllComplete()
{
}

void UFairyTransition::ApplyValue(FFairyTransitionItemBase* item)
{
}

void UFairyTransition::Setup(FairyGUI::FByteBuffer* Buffer)
{
	Name = Buffer->ReadFNameFromCache();

	Options = Buffer->ReadInt();
	bAutoPlay = Buffer->ReadBool();
	AutoPlayTimes = Buffer->ReadInt();
	AutoPlayDelay = Buffer->ReadFloat();

	int32 cnt = Buffer->ReadShort(); // item number
	for (int32 i = 0; i < cnt; i++)
	{
		int32 dataLen = Buffer->ReadShort();
		int32 curPos = Buffer->GetPos();

		Buffer->Seek(curPos, 0);

		EFairyTransitionItemType ActionType = (EFairyTransitionItemType)Buffer->ReadByte();
		FFairyTransitionItemBase* item = CreateTransitionItemWithType(ActionType); // todo: create right item object by type.
		item->SetOwner(this);
		item->Setup(Buffer, curPos);
		Items.Add(MakeShareable(item));


		Buffer->SetPos(curPos + dataLen);
	}
}

