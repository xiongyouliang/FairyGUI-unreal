#include "UI/Controller/Gears/GearDisplay2.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearDisplay2::FGearDisplay2(UFairyObject* InOwner) :
    FGearBase(InOwner),
    Condition(EConditionType::And),
    Visible(0)
{
    Type = EGearType::Display2;
}

FGearDisplay2::~FGearDisplay2()
{
}

void FGearDisplay2::Apply()
{
    if (Controller == nullptr || Pages.Num() == 0)
    {
		Visible = 1;
    }
    else
    {
        if (Pages.Contains(Controller->GetSelectedPageID()))
        {
			Visible = 1;
        }
        else
        {
			Visible = 0;
        }
    }

	if (Owner.IsValid())
	{
		Owner->CheckGearDisplay();
	}
}

bool FGearDisplay2::Evaluate(bool bConnected)
{
    bool v = Controller == nullptr || Visible > 0;
    if (Condition == EConditionType::And)
    {
		v = v && bConnected;
    }
    else
    {
		v = v || bConnected;
    }
    return v;
}

void FGearDisplay2::UpdateState()
{
}

void FGearDisplay2::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
}

void FGearDisplay2::Init()
{
    Pages.Reset();
}
