#include "UI/Controller/Gears/GearIcon.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearIcon::FGearIcon(UFairyObject * InOwner) :FGearBase(InOwner)
{
	Type = EFairyGearType::Icon;
}

FGearIcon::~FGearIcon()
{
}

void FGearIcon::Init()
{
	Default = TargetObject->GetIcon();
	Storage.Reset();
}

void FGearIcon::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	if (PageID.IsNone())
	{
		Default = Buffer->ReadStringFromCache();
	}
	else
	{
		Storage.Add(PageID, Buffer->ReadStringFromCache());
	}
}

void FGearIcon::Apply()
{
	FString* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	TargetObject->bGearLocked = true;
	TargetObject->SetIcon(*Value);
	TargetObject->bGearLocked = false;
}

void FGearIcon::UpdateState()
{
	Storage.Add(Controller->GetSelectedPageID(), TargetObject->GetIcon());
}
