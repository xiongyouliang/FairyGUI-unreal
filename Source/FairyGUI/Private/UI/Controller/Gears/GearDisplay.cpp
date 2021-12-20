#include "UI/Controller/Gears/GearDisplay.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

#define MAX_DISPLAY_LOCK_SESSION 10086

FGearDisplay::FGearDisplay(UFairyObject* InOwner) :
	FGearBase(InOwner, EFairyGearType::Display),
	Visible(0),
	DisplayLockToken(1)
{
}

FGearDisplay::~FGearDisplay()
{
}

void FGearDisplay::Apply()
{
	DisplayLockToken++;
	if (DisplayLockToken > MAX_DISPLAY_LOCK_SESSION)
	{
		DisplayLockToken = 1;
	}

	if (Pages.Num() == 0)
	{
		Visible = 1;
	}
	else
	{
		if (IsActived())
		{
			Visible = 1;
		}
		else
		{
			Visible = 0;
		}
	}
	
	if (Controller->CheckGearVisible(TargetObject.Get()))
	{
		TargetObject->SetVisible(true);
	}
	else
	{
		TargetObject->SetVisible(false);
	}
}

void FGearDisplay::UpdateState()
{
}

bool FGearDisplay::IsActived()
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

void FGearDisplay::AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer)
{
}

void FGearDisplay::Init()
{
	Pages.Reset();
}

uint32 FGearDisplay::AddLock()
{
	Visible++;
	return DisplayLockToken;
}

void FGearDisplay::ReleaseLock(uint32 token)
{
	if (token == DisplayLockToken)
	{
		Visible--;
	}
}

bool FGearDisplay::IsConnected()
{
	return IsActived();
	//return Controller == nullptr || Visible > 0;
}

bool FGearDisplay::IsOwnerVisible()
{
	bool result = false;
	if (Pages.Num() == 0)
	{
		result = true;
	}
	else
	{
		if (Controller.IsValid() && Pages.Contains(Controller->GetSelectedPageID()))
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}

	return result;
}
