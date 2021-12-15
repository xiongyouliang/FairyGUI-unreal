#include "UI/Relation/Relations.h"
#include "UI/FairyComponent.h"
#include "Utils/ByteBuffer.h"

FRelations::FRelations(UFairyObject* InOwner) :
	Handling(nullptr)
{
	Owner = InOwner;
}

FRelations::~FRelations()
{
}

void FRelations::Add(UFairyObject * InTarget, ERelationType ERelationType)
{
	Add(InTarget, ERelationType, false);
}

void FRelations::Add(UFairyObject * InTarget, ERelationType ERelationType, bool bUsePercent)
{
	verifyf(InTarget, TEXT("target is null"));

	for (auto& it : ItemList)
	{
		if (it->GetTarget() == InTarget)
		{
			it->Add(ERelationType, bUsePercent);
			return;
		}
	}
	FRelationItem* newItem = new FRelationItem(Owner);
	newItem->SetTarget(InTarget);
	newItem->Add(ERelationType, bUsePercent);
	ItemList.Add(newItem);
}

void FRelations::Remove(UFairyObject * InTarget, ERelationType ERelationType)
{
	int32 Index = 0;
	while (Index < ItemList.Num())
	{
		FRelationItem* Item = ItemList[Index];
		if (Item->GetTarget() == InTarget)
		{
			Item->Remove(ERelationType);
			if (Item->IsEmpty())
			{
				ItemList.RemoveAt(Index);
			}
			else
			{
				Index++;
			}
		}
		else
		{
			Index++;
		}
	}
}

bool FRelations::Contains(UFairyObject * InTarget)
{
	for (auto& it : ItemList)
	{
		if (it->GetTarget() == InTarget)
		{
			return true;
		}
	}

	return false;
}

void FRelations::ClearFor(UFairyObject * InTarget)
{
	int32 Index = 0;
	while (Index < ItemList.Num())
	{
		FRelationItem* Item = ItemList[Index];
		if (Item->GetTarget() == InTarget)
		{
			ItemList.RemoveAt(Index);
		}
		else
		{
			Index++;
		}
	}
}

void FRelations::ClearAll()
{
	ItemList.Reset();
}

void FRelations::CopyFrom(const FRelations & Source)
{
	ClearAll();

	for (auto& it : Source.ItemList)
	{
		FRelationItem* item = new FRelationItem(Owner);
		item->CopyFrom(*it);
		ItemList.Add(item);
	}
}

bool FRelations::IsEmpty() const
{
	return ItemList.Num() == 0;
}

void FRelations::Setup(FairyGUI::FByteBuffer* Buffer, bool bParentToChild)
{
	int32 TargetNum = Buffer->ReadByte();
	UFairyObject* TargetObject = nullptr; // the owner relations from target
	for (int32 i = 0; i < TargetNum; i++)
	{
		int16 targetIndex = Buffer->ReadShort();
		if (targetIndex == -1)
		{
			// target is container
			TargetObject = Owner->GetParent();
		}
		else if (bParentToChild)
		{
			// disable parent has relation to child
			//TargetObject = (Cast<UFairyComponent>(Owner))->GetChildAt(targetIndex);
		}
		else
		{
			// target is a brothers
			TargetObject = Owner->GetParent()->GetChildAt(targetIndex);
		}

		int32 RelationNum = Buffer->ReadByte();
		if (TargetObject && RelationNum > 0)
		{
			FRelationItem* newItem = new FRelationItem(Owner);
			for (int32 j = 0; j < RelationNum; j++)
			{
				ERelationType RelationType = (ERelationType)Buffer->ReadByte();
				bool bUsePercent = Buffer->ReadBool();
				newItem->Add(RelationType, bUsePercent);
			}
			newItem->SetTarget(TargetObject);
			ItemList.Add(newItem);
		}
	}
	if (!IsEmpty())
	{
		ApplyRelation();
	}
}

void FRelations::ApplyRelation() const
{
	checkf(!IsEmpty(), TEXT("Relations must be Not Empty"));
	FVector2D FinalSize(ForceInit);
	FRelationItem* TargetItem = ItemList[0];
	TargetItem->ApplyRelation();
}