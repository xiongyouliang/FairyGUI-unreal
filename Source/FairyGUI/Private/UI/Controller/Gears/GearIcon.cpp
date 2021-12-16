#include "UI/Controller/Gears/GearIcon.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearIcon::FGearIcon(UFairyObject * InOwner) :FGearBase(InOwner)
{
    Type = EGearType::Icon;
}

FGearIcon::~FGearIcon()
{
}

void FGearIcon::Init()
{
    Default = Owner->GetIcon();
    Storage.Reset();
}

void FGearIcon::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
    if (PageID.IsEmpty())
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

    Owner->bGearLocked = true;
    Owner->SetIcon(*Value);
    Owner->bGearLocked = false;
}

void FGearIcon::UpdateState()
{
    Storage.Add(Controller->GetSelectedPageID(), Owner->GetIcon());
}
