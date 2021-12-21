#include "UI/Controller/FairyController.h"
#include "UI/Controller/Gears/GearBase.h"
#include "UI/Controller/Gears/GearDisplay.h"
#include "UI/Controller/Gears/GearDisplay2.h"
#include "UI/FairyComponent.h"
#include "UI/FairyObject.h"
#include "Package/FairyPackage.h"
#include "Utils/ByteBuffer.h"
#include "Package/FairyPackageMgr.h"

UFairyController::UFairyController() :
	SelectedIndex(-1),
	PreviousIndex(-1)
{
}

UFairyController::~UFairyController()
{
}

void UFairyController::SetSelectedIndex(int32 Index)
{
	SetSelectedIndex(Index, true);
}

void UFairyController::SetSelectedIndex(int32 Index, bool bTriggerEvent)
{
	if (SelectedIndex != Index)
	{
		verifyf(Index < PageIDs.Num(), TEXT("Invalid selected index"));

		bChanging = true;

		PreviousIndex = SelectedIndex;
		SelectedIndex = Index;
		//Cast<UFairyComponent>(GetOuter())->ApplyController(this);
		RunActions();

		if (bTriggerEvent)
		{
			Apply();
		}

		bChanging = false;
	}
}

FName UFairyController::GetSelectedPage() const
{
	if (SelectedIndex == -1)
	{
		return NAME_None;
	}
	else
	{
		return PageNames[SelectedIndex];
	}
}

void UFairyController::SetSelectedPage(const FName& PageName)
{
	SetSelectedPage(PageName, true);
}

void UFairyController::SetSelectedPage(const FName& PageName, bool bTriggerEvent)
{
	int32 i = PageNames.Find(PageName);
	if (i == INDEX_NONE)
	{
		i = 0;
	}
	SetSelectedIndex(i, bTriggerEvent);
}

FName UFairyController::GetSelectedPageID() const
{
	if (SelectedIndex == -1)
	{
		return NAME_None;
	}
	else
	{
		return PageIDs[SelectedIndex];
	}
}

void UFairyController::SetSelectedPageID(const FName& PageID, bool bTriggerEvent)
{
	int32 i = PageIDs.Find(PageID);
	if (i != INDEX_NONE)
	{
		SetSelectedIndex(i, bTriggerEvent);
	}
}

FName UFairyController::GetPreviousPage() const
{
	if (PreviousIndex == -1)
	{
		return NAME_None;
	}
	else
	{
		return PageNames[PreviousIndex];
	}
}

FName UFairyController::GetPreviousPageID() const
{
	if (PreviousIndex == -1)
	{
		return NAME_None;
	}
	else
	{
		return PageIDs[PreviousIndex];
	}
}

int32 UFairyController::GetPageCount() const
{
	return PageIDs.Num();
}

bool UFairyController::HasPage(const FName& PageName) const
{
	return PageNames.Find(PageName) != INDEX_NONE;
}

int32 UFairyController::GetPageIndexByID(const FName& PageID) const
{
	return PageIDs.Find(PageID) != INDEX_NONE;
}

FName UFairyController::GetPageNameByID(const FName& PageID) const
{
	int32 i = PageIDs.Find(PageID);
	if (i != INDEX_NONE)
	{
		return PageNames[i];
	}
	else
	{
		return NAME_None;
	}
}

FName UFairyController::GetPageID(int32 Index) const
{
	return PageIDs[Index];
}

void UFairyController::SetOppositePageID(const FName& PageID)
{
	int32 i = PageIDs.Find(PageID);
	if (i > 0)
	{
		SetSelectedIndex(0);
	}
	else if (PageIDs.Num() > 1)
	{
		SetSelectedIndex(1);
	}
}

void UFairyController::AddAbserver(TSharedPtr<FGearBase> InPenddingAddGear)
{
	ObserverList.Push(InPenddingAddGear);
}

void UFairyController::RemoveAbserver(TSharedPtr<FGearBase> InPenddingRemoveGear)
{
	for (size_t i = 0; i < ObserverList.Num(); i++)
	{
		if (ObserverList[i].Get() == InPenddingRemoveGear.Get())
		{
			ObserverList.RemoveAt(i);
		}
	}
}

void UFairyController::RemoveAllAbserver()
{
	ObserverList.Empty();
}

void UFairyController::Apply()
{
	for (size_t i = 0; i < ObserverList.Num(); i++)
	{
		TSharedPtr<FGearBase> Observer = ObserverList[i];
		if (Observer.IsValid())
		{
			Observer->Apply();
		}
	}
}

bool UFairyController::CheckGearVisible(const UFairyObject* InTarget)
{
	if (!InTarget)
	{
		return false;
	}

	FGearDisplay* DisplayGear1 = nullptr;
	FGearDisplay2* DisplayGear2 = nullptr;
	for (size_t i = 0; i < ObserverList.Num(); i++)
	{
		FGearBase* Gear = ObserverList[i].Get();
		if (Gear->GetTarget() == InTarget)
		{
			if (Gear->GetType() == EFairyGearType::Display)
			{
				DisplayGear1 = static_cast<FGearDisplay*>(Gear);
			}

			if (Gear->GetType() == EFairyGearType::Display2)
			{
				DisplayGear2 = static_cast<FGearDisplay2*>(Gear);
			}
		}
	}

	if (DisplayGear1 && DisplayGear2)
	{
		return DisplayGear2->Evaluate(DisplayGear1->IsActived());
	}
	else if (DisplayGear1)
	{
		return DisplayGear1->IsActived();
	}

	return false;
}

void UFairyController::RunActions()
{
	if (Actions.Num() == 0)
	{
		return;
	}

	for (auto& it : Actions)
	{
		it.Run(this, GetPreviousPageID().ToString(), GetSelectedPageID().ToString());
	}
}

void UFairyController::Setup(FairyGUI::FByteBuffer* Buffer)
{
	int32 BeginPos = Buffer->GetPos();
	Buffer->Seek(BeginPos, 0);

	Name = Buffer->ReadFNameFromCache();
	bAutoRadioGroupDepth = Buffer->ReadBool();

	Buffer->Seek(BeginPos, 1);

	// Parse pages define in this Controller
	int32 cnt = Buffer->ReadShort();
	PageIDs.SetNum(cnt);
	PageNames.SetNum(cnt);
	for (int32 i = 0; i < cnt; i++)
	{
		PageIDs[i] = Buffer->ReadFNameFromCache();
		PageNames[i] = Buffer->ReadFNameFromCache();
	}

	// Parse HomePage property for this Controller
	int32 HomePageIndex = 0; // The default home page type is: EHomePageType::FirstPage = 0;
	if (Buffer->Version >= 2)
	{
		EHomePageType HomePageType = (EHomePageType)( Buffer->ReadByte() );
		switch (HomePageType)
		{
		case EHomePageType::Specific:
			HomePageIndex = Buffer->ReadShort();
			break;

		case EHomePageType::MatchBranch:
			HomePageIndex = PageNames.Find(UFairyPackageMgr::Get()->GetBranch());
			if (HomePageIndex == INDEX_NONE)
			{
				HomePageIndex = 0;
			}
			break;

		case EHomePageType::MatchVariable:
			HomePageIndex = PageNames.Find(UFairyPackageMgr::Get()->GetVar(Buffer->ReadFNameFromCache()));
			if (HomePageIndex == INDEX_NONE)
			{
				HomePageIndex = 0;
			}
			break;
		}
	}

	// Parse action define in this controller
	Buffer->Seek(BeginPos, 2);
	cnt = Buffer->ReadShort();
	if (cnt > 0)
	{
		for (int32 i = 0; i < cnt; i++)
		{
			int32 nextPos = Buffer->ReadShort();
			nextPos += Buffer->GetPos();

			FControllerAction* Action = FControllerAction::CreateAction(Buffer->ReadByte());
			Action->Setup(Buffer);
			Actions.Add(Action);

			Buffer->SetPos(nextPos);
		}
	}

	if (PageIDs.Num() > 0)
	{
		SetSelectedIndex(HomePageIndex);
	}
	else
	{
		SelectedIndex = -1;
	}
}

