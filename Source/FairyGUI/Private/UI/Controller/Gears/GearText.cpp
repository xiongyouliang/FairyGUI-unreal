#include "UI/Controller/Gears/GearText.h"
#include "UI/FairyObject.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearText::FGearText(UFairyObject* InOwner) : FGearBase(InOwner)
{
    Type = EGearType::Text;
}

FGearText::~FGearText()
{
}

void FGearText::Init()
{
    Default = Owner->GetText();
    Storage.Reset();
}

void FGearText::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
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

void FGearText::Apply()
{
    FString* Value = Storage.Find(Controller->GetSelectedPageID());
    if (Value == nullptr)
    {
		Value = &Default;
    }

    Owner->bGearLocked = true;
    Owner->SetText(*Value);
    Owner->bGearLocked = false;
}

void FGearText::UpdateState()
{
    Storage.Add(Controller->GetSelectedPageID(), Owner->GetText());
}
