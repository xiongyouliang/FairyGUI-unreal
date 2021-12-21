#include "UI/Controller/Gears/GearText.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearText::FGearText(UFairyObject* InOwner) : FGearBase(InOwner)
{
	Type = EFairyGearType::Text;
}

FGearText::~FGearText()
{
}

void FGearText::Init()
{
	Default = TargetObject->GetText();
	Storage.Reset();
}

void FGearText::AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer)
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

void FGearText::Apply()
{
	FString* Value = Storage.Find(Controller->GetSelectedPageID());
	if (Value == nullptr)
	{
		Value = &Default;
	}

	TargetObject->bGearLocked = true;
	TargetObject->SetText(*Value);
	TargetObject->bGearLocked = false;
}

void FGearText::UpdateState()
{
	Storage.Add(Controller->GetSelectedPageID(), TargetObject->GetText());
}
