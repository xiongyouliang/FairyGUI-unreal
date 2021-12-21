#include "UI/Controller/Gears/GearFontSize.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearFontSize::FGearFontSize(UFairyObject* InOwner) : FGearBase(InOwner)
{
	Type = EFairyGearType::FontSize;
}

FGearFontSize::~FGearFontSize()
{
}

void FGearFontSize::Init()
{
	Default = TargetObject->GetProp<int32>(EObjectPropID::FontSize);
	Storage.Reset();
}

void FGearFontSize::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
{
	if (PageID.IsNone())
	{
		Default = Buffer->ReadInt();
	}
	else
	{
		Storage.Add(PageID, Buffer->ReadInt());
	}
}

void FGearFontSize::Apply()
{
	int32* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	TargetObject->bGearLocked = true;
	TargetObject->SetProp(EObjectPropID::FontSize, FNVariant(*Value));
	TargetObject->bGearLocked = false;
}

void FGearFontSize::UpdateState()
{
	Storage.Add(Controller->GetSelectedPageID(), TargetObject->GetProp<int32>(EObjectPropID::FontSize));
}
