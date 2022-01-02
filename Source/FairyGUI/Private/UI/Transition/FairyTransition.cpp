#include "UI/Transition/FairyTransition.h"
#include "FairyApplication.h"
#include "Package/FairyPackage.h"
#include "Utils/ByteBuffer.h"

#include "UI/FairyComponent.h"
#include "UI/Controller/FairyController.h"
#include "UI/Transition/FairyTransitionTimeline.h"
#include "UI/Tween/GPath.h"


const int32 OPTION_IGNORE_DISPLAY_CONTROLLER = 1;
const int32 OPTION_AUTO_STOP_DISABLED = 2;
const int32 OPTION_AUTO_STOP_AT_END = 4;


UFairyTransition::UFairyTransition() :
	bPlaying(false),
	bPaused(false),
	bAutoPlay(false),
	Options(0),
	RepeatTimes(0)
{

}

UFairyTransition::~UFairyTransition()
{

}

FName UFairyTransition::GetName()
{
	return Name;
}

void UFairyTransition::OnTargetComponentEnter()
{
	if (bAutoPlay)
	{
		for (size_t i = 0; i < TimelineList.Num(); i++)
		{
			TSharedPtr<FFairyTransitionTimeline> Timeline = TimelineList[i];
			if (Timeline.IsValid())
			{
				Timeline->Play();
			}
		}
	}
}

void UFairyTransition::OnTargetComponentExit()
{
}

TSharedPtr<FFairyTransitionTimeline> UFairyTransition::GetOrAddTimeline(EFairyTransitionItemType InType, UFairyObject* InObject)
{
	
	for (size_t i = 0; i < TimelineList.Num(); i++)
	{
		TSharedPtr<FFairyTransitionTimeline> Timeline = TimelineList[i];
		if (Timeline->GetTarget() == InObject && Timeline->GetTimelineType() == InType)
		{
			return Timeline;
		}
	}

	TSharedPtr<FFairyTransitionTimeline> Timeline = MakeShareable(new FFairyTransitionTimeline);
	Timeline->SetOwner(this);
	Timeline->SetTarget(InObject);
	Timeline->SetTimelineType(InType);

	TimelineList.Push(Timeline);
	return Timeline;
}

void UFairyTransition::AddTransitionItem(FFairyTransitionItemBase* InTransitionItem)
{
	TSharedPtr<FFairyTransitionTimeline> timeline = GetOrAddTimeline(InTransitionItem->GetTimelineType(), InTransitionItem->GetTarget());
	timeline->AddTransitionItem(InTransitionItem);
}

void UFairyTransition::Setup(FairyGUI::FByteBuffer* Buffer)
{
	Name = Buffer->ReadFNameFromCache();

	Options = Buffer->ReadInt();
	bAutoPlay = Buffer->ReadBool();
	RepeatTimes = Buffer->ReadInt();
	AutoPlayDelay = Buffer->ReadFloat();

	int32 ItemNum = Buffer->ReadShort(); // item number
	for (int32 i = 0; i < ItemNum; i++)
	{
		int32 dataLen = Buffer->ReadShort();
		int32 curPos = Buffer->GetPos();

		Buffer->Seek(curPos, 0);

		EFairyTransitionItemType ActionType = (EFairyTransitionItemType)Buffer->ReadByte();
		FFairyTransitionItemBase* item = CreateTransitionItemWithType(ActionType);
		item->SetOwner(this);
		item->Setup(Buffer, curPos);

		this->AddTransitionItem(item);

		Buffer->SetPos(curPos + dataLen);
	}
}

