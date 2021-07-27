#include "UI/Relations.h"
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

    for (auto& it : Items)
    {
        if (it.GetTarget() == InTarget)
        {
            it.Add(ERelationType, bUsePercent);
            return;
        }
    }
    FRelationItem* newItem = new FRelationItem(Owner);
    newItem->SetTarget(InTarget);
    newItem->Add(ERelationType, bUsePercent);
    Items.Add(newItem);
}

void FRelations::Remove(UFairyObject * InTarget, ERelationType ERelationType)
{
    int32 Index = 0;
    while (Index < Items.Num())
    {
        FRelationItem& Item = Items[Index];
        if (Item.GetTarget() == InTarget)
        {
            Item.Remove(ERelationType);
            if (Item.IsEmpty())
            {
                Items.RemoveAt(Index);
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
    for (auto& it : Items)
    {
        if (it.GetTarget() == InTarget)
        {
            return true;
        }
    }

    return false;
}

void FRelations::ClearFor(UFairyObject * InTarget)
{
    int32 Index = 0;
    while (Index < Items.Num())
    {
        FRelationItem& Item = Items[Index];
        if (Item.GetTarget() == InTarget)
        {
            Items.RemoveAt(Index);
        }
        else
        {
            Index++;
        }
    }
}

void FRelations::ClearAll()
{
    Items.Reset();
}

void FRelations::CopyFrom(const FRelations & Source)
{
    ClearAll();

    for (auto& it : Source.Items)
    {
        FRelationItem* item = new FRelationItem(Owner);
        item->CopyFrom(it);
        Items.Add(item);
    }
}

void FRelations::OnOwnerSizeChanged(const FVector2D& Delta, bool bApplyPivot)
{
    for (auto& it : Items)
    {
        it.ApplySelfSizeChanged(Delta.X, Delta.Y, bApplyPivot);
    }
}

bool FRelations::IsEmpty() const
{
    return Items.Num() == 0;
}

void FRelations::Setup(FByteBuffer * Buffer, bool bParentToChild)
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
                newItem->InternalAdd(RelationType, bUsePercent);
            }
            newItem->SetTarget(TargetObject);
            Items.Add(newItem);
        }
    }
}

FVector2D FRelations::GetRelationSize() const
{
    checkf(!IsEmpty(), TEXT("Relations must be Not Empty"));
    FVector2D FinalSize(ForceInit);
    FRelationItem TargetItem = Items[0];
    FinalSize = TargetItem.GetRelationSize(); // todo: guess only has one relation target

    return FinalSize;
}

FVector2D FRelations::GetRelationPos() const
{
    checkf(!IsEmpty(), TEXT("Relations must be Not Empty"));
    FVector2D FinalPos;
    FRelationItem TargetItem = Items[0];

    FinalPos = TargetItem.GetRelationPos(); // todo: guess only has one relation target

    return FinalPos;
}