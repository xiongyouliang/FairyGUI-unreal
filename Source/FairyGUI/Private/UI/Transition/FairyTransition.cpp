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
	Options(0),
	AutoPlayTimes(0)
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
		for (size_t i = 0; i < Items.Num(); i++)
		{
			TSharedPtr<FFairyTransitionItemBase> itemPtr = Items[i];
			if (itemPtr.IsValid())
			{
				itemPtr->RunItem();
			}
		}
	}
}

void UFairyTransition::OnTargetComponentExit()
{
}

void UFairyTransition::Setup(FairyGUI::FByteBuffer* Buffer)
{
	Name = Buffer->ReadFNameFromCache();

	Options = Buffer->ReadInt();
	bAutoPlay = Buffer->ReadBool();
	AutoPlayTimes = Buffer->ReadInt();
	AutoPlayDelay = Buffer->ReadFloat();

	int32 ItemNum = Buffer->ReadShort(); // item number
	for (int32 i = 0; i < ItemNum; i++)
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

