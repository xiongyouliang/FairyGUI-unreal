#include "UI/Controller/Gears/GearDisplay2.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

FGearDisplay2::FGearDisplay2(UFairyObject* InOwner) :
	FGearBase(InOwner),
	Condition(EConditionType::And)
{
	Type = EFairyGearType::Display2;
}

FGearDisplay2::~FGearDisplay2()
{
}

void FGearDisplay2::Apply()
{
	/**
	* this is a repreat logic code.
	*	if has a GearDisplay2 object binding a UFairyObject and a UFairyController, 
	*	must has a GearDisplay1 exist, GearDisplay1 Apply() logic will check GearDisplay2 state.
	*/
	//if (Controller.Get()->CheckGearVisible(TargetObject.Get()))
	//{
	//	TargetObject->SetVisible(true);
	//}
	//else
	//{
	//	TargetObject->SetVisible(false);
	//}
}

bool FGearDisplay2::Evaluate(bool bConnected)
{
	bool result = false;
	if (Condition == EConditionType::And)
	{
		result = IsActived() && bConnected;
	}
	else
	{
		result = IsActived() || bConnected;
	}
	return result;
}

void FGearDisplay2::UpdateState()
{
}

bool FGearDisplay2::IsActived()
{
	if (Controller.IsValid() && Pages.Contains(Controller->GetSelectedPageID()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void FGearDisplay2::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
}

void FGearDisplay2::Init()
{
	Pages.Reset();
}
