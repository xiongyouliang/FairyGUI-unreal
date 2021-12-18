// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controller/FairyControllerMgr.h"

void UFairyControllerArrayWrapper::AddController(UFairyController* InFairyControlller)
{
	ControllerList.Push(InFairyControlller);
}

void UFairyControllerArrayWrapper::RemoveAllController()
{
	ControllerList.Empty();
}

TArray<UFairyController*>& UFairyControllerArrayWrapper::GetControllerList()
{
	return ControllerList;
}

void UFairyControllerMgr::AddController(UFairyComponent* InTarget, UFairyController* InFairyController)
{
	if (!EntryTable.Contains(InTarget))
	{
		EntryTable.Add(InTarget, NewObject<UFairyControllerArrayWrapper>(this));
	}
	UFairyControllerArrayWrapper* ControllerArray = EntryTable.FindOrAdd(InTarget);
	ControllerArray->AddController(InFairyController);
}

void UFairyControllerMgr::RemoveController(UFairyComponent* InTarget, UFairyController* InPenddingRemoveController)
{
	UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
	if (ControllerArray)
	{
		TArray<UFairyController*>& ControllerList = ControllerArray->GetControllerList();
		for (size_t i = 0; i < ControllerList.Num(); i++)
		{
			UFairyController* Controller = ControllerList[i];
			if (Controller == InPenddingRemoveController)
			{
				ControllerList.RemoveAt(i);
				return;
			}
		}
	}
}

void UFairyControllerMgr::RemoveAllController(UFairyComponent* InTarget)
{
	if (EntryTable.Contains(InTarget))
	{
		UFairyControllerArrayWrapper* ControllerArray = EntryTable.FindOrAdd(InTarget);
		ControllerArray->RemoveAllController();

		EntryTable.Remove(InTarget);
	}
}

void UFairyControllerMgr::ApplyAllController(UFairyComponent* InTarget)
{
	if (EntryTable.Contains(InTarget))
	{
		UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
		if (ControllerArray)
		{
			TArray<UFairyController*>& ControllerList = ControllerArray->GetControllerList();
			for (size_t i = 0; i < ControllerList.Num(); i++)
			{
				UFairyController* Controller = ControllerList[i];
				Controller->Apply();
			}
		}
	}
}

void UFairyControllerMgr::ApplyController(UFairyComponent* InTarget, UFairyController* InPenddingApplyController)
{
	InPenddingApplyController->Apply();
}

void UFairyControllerMgr::ApplyController(UFairyComponent* InTarget, const FName& InName)
{
	if (EntryTable.Contains(InTarget))
	{
		UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
		if (ControllerArray)
		{
			TArray<UFairyController*>& ControllerList = ControllerArray->GetControllerList();
			for (size_t i = 0; i < ControllerList.Num(); i++)
			{
				UFairyController* Controller = ControllerList[i];
				if (Controller->GetName().IsEqual(InName))
				{
					Controller->Apply();
				}
			}
		}
	}
}

bool UFairyControllerMgr::HasController(const UFairyComponent* InTarget)
{
	if (EntryTable.Contains(InTarget))
	{
		return GetControllerList(InTarget).Num() > 0;
	}
	return false;
}

UFairyController* UFairyControllerMgr::GetController(const UFairyComponent* InTarget, FName InName)
{
	if (EntryTable.Contains(InTarget))
	{
		UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
		if (ControllerArray)
		{
			TArray<UFairyController*>& ControllerList = ControllerArray->GetControllerList();
			for (size_t i = 0; i < ControllerList.Num(); i++)
			{
				UFairyController* Controller = ControllerList[i];
				if (Controller->GetName().IsEqual(InName))
				{
					return Controller;
				}
			}
		}
	}
	return nullptr;
}

UFairyController* UFairyControllerMgr::GetController(const UFairyComponent* InTarget, int32 InIndex)
{
	if (EntryTable.Contains(InTarget))
	{
		UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
		if (ControllerArray)
		{
			TArray<UFairyController*>& ControllerList = ControllerArray->GetControllerList();
			for (size_t i = 0; i < ControllerList.Num(); i++)
			{
				if (i == InIndex)
				{
					return ControllerList[i];
				}
			}
		}
	}
	return nullptr;
}

const TArray<UFairyController*>& UFairyControllerMgr::GetControllerList(const UFairyComponent* InTarget)
{
	UFairyControllerArrayWrapper* ControllerArray = EntryTable[InTarget];
	return ControllerArray->GetControllerList();
}