#include "UI/RelationItem.h"
#include "UI/Relations.h"
#include "UI/FairyComponent.h"
#include "UI/GGroup.h"
#include "UI/Transition.h"

FRelationItem::FRelationItem(UFairyObject* InOwner) :
    TargetData(ForceInit),
    Target(nullptr)
{
    Owner = InOwner;
}

FRelationItem::~FRelationItem()
{
    ReleaseRefTarget();
}

void FRelationItem::SetTarget(UFairyObject* InTarget)
{
    if (Target != InTarget)
    {
        ReleaseRefTarget();
        Target = InTarget;
        if (InTarget)
        {
            AddRefTarget(InTarget);
        }
    }
}

void FRelationItem::Add(ERelationType RelationType, bool bUsePercent)
{
    if (RelationType == ERelationType::Size)
    {
        Add(ERelationType::Width, bUsePercent);
        Add(ERelationType::Height, bUsePercent);
        return;
    }

    for (auto& it : Defs)
    {
        if (it.Type == RelationType)
        {
            return;
        }
    }

    InternalAdd(RelationType, bUsePercent);
}

void FRelationItem::InternalAdd(ERelationType RelationType, bool bUsePercent)
{
    if (RelationType == ERelationType::Size)
    {
        InternalAdd(ERelationType::Width, bUsePercent);
        InternalAdd(ERelationType::Height, bUsePercent);
        return;
    }

    FRelationDef info;
    info.bPercent = bUsePercent;
    info.Type = RelationType;
    info.Axis = (RelationType <= ERelationType::Right_Right || RelationType == ERelationType::Width || (RelationType >= ERelationType::LeftExt_Left && RelationType <= ERelationType::RightExt_Right)) ? 0 : 1;
    Defs.Add(info);
}

void FRelationItem::Remove(ERelationType RelationType)
{
    if (RelationType == ERelationType::Size)
    {
        Remove(ERelationType::Width);
        Remove(ERelationType::Height);
        return;
    }

    int32 i = 0;
    for (auto& def : Defs)
    {
        if (def.Type == RelationType)
        {
            Defs.RemoveAt(i);
            break;
        }
        i++;
    }
}

void FRelationItem::CopyFrom(const FRelationItem& Source)
{
    SetTarget(Source.Target);

    Defs.Reset();
    for (auto& it : Source.Defs)
    {
        Defs.Add(it);
    }
}

bool FRelationItem::IsEmpty() const
{
    return Defs.Num() == 0;
}

void FRelationItem::ApplyOnSelfSizeChanged(float DeltaWidth, float DeltaHeight, bool bApplyPivot)
{
    if (Target == nullptr || Defs.Num() == 0)
    {
        return;
    }

    const FVector2D OldPos = Owner->GetPosition();

    for (auto& it : Defs)
    {
        switch (it.Type)
        {
        case ERelationType::Center_Center:
            //Owner->SetPositionX(OldPos.X - (0.5 - (bApplyPivot ? Owner->Pivot.X : 0)) * DeltaWidth);
            break;

        case ERelationType::Right_Center:
        case ERelationType::Right_Left:
        case ERelationType::Right_Right:
            //Owner->SetPositionX(OldPos.X - (1 - (bApplyPivot ? Owner->Pivot.X : 0)) * DeltaWidth);
            break;

        case ERelationType::Middle_Middle:
            //Owner->SetPositionY(OldPos.Y - (0.5 - (bApplyPivot ? Owner->Pivot.Y : 0)) * DeltaHeight);
            break;

        case ERelationType::Bottom_Middle:
        case ERelationType::Bottom_Top:
        case ERelationType::Bottom_Bottom:
            //Owner->SetPositionY(OldPos.Y - (1 - (bApplyPivot ? Owner->Pivot.Y : 0)) * DeltaHeight);
            break;

        default:
            break;
        }
    }

    if (OldPos != Owner->GetPosition())
    {
        FVector2D Delta = Owner->GetPosition() - OldPos;

        Owner->UpdateGearFromRelations(1, Delta);

        if (Owner->HasParent())
        {
            const TArray<UTransition*>& arr = Owner->GetParent()->GetTransitions();
            for (auto& it : arr)
            {
                it->UpdateFromRelations(Owner->GetID(), Delta);
            }
        }
    }
}

void FRelationItem::ApplyOnXYChanged(UFairyObject* InTarget, const FRelationDef& info, float dx, float dy)
{
    float tmp;
    const FVector2D OldPos = Owner->GetPosition();

    switch (info.Type)
    {
    case ERelationType::Left_Left:
    case ERelationType::Left_Center:
    case ERelationType::Left_Right:
    case ERelationType::Center_Center:
    case ERelationType::Right_Left:
    case ERelationType::Right_Center:
    case ERelationType::Right_Right:
        Owner->SetPositionX(OldPos.X + dx);
        break;

    case ERelationType::Top_Top:
    case ERelationType::Top_Middle:
    case ERelationType::Top_Bottom:
    case ERelationType::Middle_Middle:
    case ERelationType::Bottom_Top:
    case ERelationType::Bottom_Middle:
    case ERelationType::Bottom_Bottom:
        Owner->SetPositionY(OldPos.Y + dy);
        break;

    case ERelationType::Width:
    case ERelationType::Height:
        break;

    case ERelationType::LeftExt_Left:
    case ERelationType::LeftExt_Right:
        if (Owner != InTarget->GetParent())
        {
            tmp = Owner->GetMinWidth();
            //Owner->SetWidth(Owner->RawSize.X - dx);
            Owner->SetMinWidth(tmp + dx);
        }
        else
        {
            //Owner->SetWidth(Owner->RawSize.X - dx);
        }
        break;

    case ERelationType::RightExt_Left:
    case ERelationType::RightExt_Right:
        if (Owner != InTarget->GetParent())
        {
            tmp = Owner->GetMinWidth();
            //Owner->SetWidth(Owner->RawSize.X + dx);
            Owner->SetMinWidth(tmp);
        }
        else
            //Owner->SetWidth(Owner->RawSize.X + dx);
        break;

    case ERelationType::TopExt_Top:
    case ERelationType::TopExt_Bottom:
        if (Owner != InTarget->GetParent())
        {
            tmp = Owner->GetMinHeight();
            //Owner->SetHeight(Owner->RawSize.Y - dy);
            Owner->SetMinHeight(tmp + dy);
        }
        else
        {
            //Owner->SetHeight(Owner->RawSize.Y - dy);
        }
        break;

    case ERelationType::BottomExt_Top:
    case ERelationType::BottomExt_Bottom:
        if (Owner != InTarget->GetParent())
        {
            tmp = Owner->GetMinHeight();
            //Owner->SetHeight(Owner->RawSize.Y + dy);
            Owner->SetMinHeight(tmp);
        }
        else
        {
            //Owner->SetHeight(Owner->RawSize.Y + dy);
        }
        break;

    default:
        break;
    }
}

void FRelationItem::ApplyOnSizeChanged(UFairyObject* InTarget, const FRelationDef& info)
{
    float pos = 0, pivot = 0, delta = 0;
    if (info.Axis == 0)
    {
        if (InTarget != Owner->GetParent())
        {
            pos = InTarget->GetPosition().X;
            //if (InTarget->bPivotAsAnchor)
            //{
            //    pivot = InTarget->Pivot.X;
            //}
        }

        if (info.bPercent)
        {
            if (TargetData.Z != 0)
            {
                delta = InTarget->GetWidth() / TargetData.Z;
            }
        }
        else
        {
            delta = InTarget->GetWidth() - TargetData.Z;
        }
    }
    else
    {
        if (InTarget != Owner->GetParent())
        {
            pos = InTarget->GetPosition().Y;
            //if (InTarget->bPivotAsAnchor)
            //{
            //    pivot = InTarget->Pivot.Y;
            //}
        }

        if (info.bPercent)
        {
            if (TargetData.W != 0)
            {
                delta = InTarget->GetHeight() / TargetData.W;
            }
        }
        else
        {
            delta = InTarget->GetHeight() - TargetData.W;
        }
    }

    float v, tmp;

    switch (info.Type)
    {
    case ERelationType::Left_Left:
        if (info.bPercent)
        {
            Owner->SetMinWidth(pos + (Owner->GetMinWidth() - pos) * delta);
        }
        else if (pivot != 0)
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (-pivot));
        }
        break;
    case ERelationType::Left_Center:
        if (info.bPercent)
        {
            Owner->SetMinWidth(pos + (Owner->GetMinWidth() - pos) * delta);
        }
        else
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Left_Right:
        if (info.bPercent)
        {
            Owner->SetMinWidth(pos + (Owner->GetMinWidth() - pos) * delta);
        }
        else
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (1 - pivot));
        }
        break;
    case ERelationType::Center_Center:
        if (info.bPercent)
        {
            //Owner->SetMinWidth(pos + (Owner->GetMinWidth() + Owner->RawSize.X * 0.5f - pos) * delta - Owner->RawSize.X * 0.5f);
        }
        else
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Right_Left:
        if (info.bPercent)
        {
            //Owner->SetMinWidth(pos + (Owner->GetMinWidth() + Owner->RawSize.X - pos) * delta - Owner->RawSize.X);
        }
        else if (pivot != 0)
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (-pivot));
        }
        break;
    case ERelationType::Right_Center:
        if (info.bPercent)
        {
            //Owner->SetMinWidth(pos + (Owner->GetMinWidth() + Owner->RawSize.X - pos) * delta - Owner->RawSize.X);
        }
        else
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Right_Right:
        if (info.bPercent)
        {
            //Owner->SetMinWidth(pos + (Owner->GetMinWidth() + Owner->RawSize.X - pos) * delta - Owner->RawSize.X);

        }
        else
        {
            Owner->SetPositionX(Owner->GetPosition().X + delta * (1 - pivot));
        }
        break;
    case ERelationType::Top_Top:
        if (info.bPercent)
        {
            Owner->SetMinHeight(pos + (Owner->GetMinHeight() - pos) * delta);
        }
        else if (pivot != 0)
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (-pivot));
        }
        break;
    case ERelationType::Top_Middle:
        if (info.bPercent)
        {
            Owner->SetMinHeight(pos + (Owner->GetMinHeight() - pos) * delta);
        }
        else
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Top_Bottom:
        if (info.bPercent)
        {
            Owner->SetMinHeight(pos + (Owner->GetMinHeight() - pos) * delta);
        }
        else
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (1 - pivot));
        }
        break;
    case ERelationType::Middle_Middle:
        if (info.bPercent)
        {
            //Owner->SetMinHeight(pos + (Owner->GetMinHeight() + Owner->RawSize.Y * 0.5f - pos) * delta - Owner->RawSize.Y * 0.5f);

        }
        else
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Bottom_Top:
        if (info.bPercent)
        {
            //Owner->SetMinHeight(pos + (Owner->GetMinHeight() + Owner->RawSize.Y - pos) * delta - Owner->RawSize.Y);
        }
        else if (pivot != 0)
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (-pivot));
        }
        break;
    case ERelationType::Bottom_Middle:
        if (info.bPercent)
        {
            //Owner->SetMinHeight(pos + (Owner->GetMinHeight() + Owner->RawSize.Y - pos) * delta - Owner->RawSize.Y);

        }
        else
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (0.5f - pivot));
        }
        break;
    case ERelationType::Bottom_Bottom:
        if (info.bPercent)
        {
            //Owner->SetMinHeight(pos + (Owner->GetMinHeight() + Owner->RawSize.Y - pos) * delta - Owner->RawSize.Y);
        }
        else
        {
            Owner->SetPositionY(Owner->GetPosition().Y + delta * (1 - pivot));
        }
        break;

    case ERelationType::Width:
        if (Owner->bUnderConstruct && Owner == InTarget->GetParent())
        {
            v = Owner->GetSize().X - InTarget->GetSize().X;
        }
        else
        {
            //v = Owner->RawSize.X - TargetData.Z;
        }
        if (info.bPercent)
        {
            v = v * delta;
        }
        if (InTarget == Owner->GetParent())
        {
            //if (Owner->bPivotAsAnchor)
            //{
            //    tmp = Owner->GetMinWidth();
            //    Owner->SetSize(FVector2D(InTarget->GetWidth() + v, Owner->RawSize.Y), true);
            //    Owner->SetMinWidth(tmp);
            //}
            //else
            //{
            //    Owner->SetSize(FVector2D(InTarget->GetWidth() + v, Owner->RawSize.Y), true);
            //}
        }
        else
        {
            Owner->SetWidth(InTarget->GetWidth() + v);
        }
        break;
    case ERelationType::Height:
        if (Owner->bUnderConstruct && Owner == InTarget->GetParent())
        {
            v = Owner->GetSize().Y - InTarget->GetSize().Y;
        }
        else
        {
            //v = Owner->RawSize.Y - TargetData.W;
        }
        if (info.bPercent)
        {
            v = v * delta;
        }
        if (InTarget == Owner->GetParent())
        {
            //if (Owner->bPivotAsAnchor)
            //{
            //    tmp = Owner->GetMinHeight();
            //    Owner->SetSize(FVector2D(Owner->RawSize.X, InTarget->GetHeight() + v), true);
            //    Owner->SetMinHeight(tmp);
            //}
            //else
            //{
            //    Owner->SetSize(FVector2D(Owner->RawSize.X, InTarget->GetHeight() + v), true);
            //}
        }
        else
            Owner->SetHeight(InTarget->GetHeight() + v);
        break;

    case ERelationType::LeftExt_Left:
        tmp = Owner->GetMinWidth();
        if (info.bPercent)
        {
            v = pos + (tmp - pos) * delta - tmp;
        }
        else
        {
            v = delta * (-pivot);
        }
        //Owner->SetWidth(Owner->RawSize.X - v);
        Owner->SetMinWidth(tmp + v);
        break;
    case ERelationType::LeftExt_Right:
        tmp = Owner->GetMinWidth();
        if (info.bPercent)
        {
            v = pos + (tmp - pos) * delta - tmp;

        }
        else
        {
            v = delta * (1 - pivot);
        }
        //Owner->SetWidth(Owner->RawSize.X - v);
        Owner->SetMinWidth(tmp + v);
        break;
    case ERelationType::RightExt_Left:
        tmp = Owner->GetMinWidth();
        if (info.bPercent)
        {
            //v = pos + (tmp + Owner->RawSize.X - pos) * delta - (tmp + Owner->RawSize.X);
        }
        else
        {
            v = delta * (-pivot);

        }
        //Owner->SetWidth(Owner->RawSize.X + v);
        Owner->SetMinWidth(tmp);
        break;
    case ERelationType::RightExt_Right:
        tmp = Owner->GetMinWidth();
        if (info.bPercent)
        {
            if (Owner == InTarget->GetParent())
            {
                if (Owner->bUnderConstruct)
                {
                    Owner->SetWidth(pos + InTarget->GetWidth() - InTarget->GetWidth() * pivot +
                        (Owner->GetSize().X - pos - InTarget->GetSize().X + InTarget->GetSize().X * pivot) * delta);
                }
                else
                {
                    //Owner->SetWidth(pos + (Owner->RawSize.X - pos) * delta);
                }
            }
            else
            {
                //v = pos + (tmp + Owner->RawSize.X - pos) * delta - (tmp + Owner->RawSize.X);
                //Owner->SetWidth(Owner->RawSize.X + v);
                Owner->SetMinWidth(tmp);
            }
        }
        else
        {
            if (Owner == InTarget->GetParent())
            {
                if (Owner->bUnderConstruct)
                {
                    Owner->SetWidth(Owner->GetSize().X + (InTarget->GetWidth() - InTarget->GetSize().X) * (1 - pivot));
                }
                else
                {
                    //Owner->SetWidth(Owner->RawSize.X + delta * (1 - pivot));
                }
            }
            else
            {
                v = delta * (1 - pivot);
                //Owner->SetWidth(Owner->RawSize.X + v);
                Owner->SetMinWidth(tmp);
            }
        }
        break;
    case ERelationType::TopExt_Top:
        tmp = Owner->GetMinHeight();
        if (info.bPercent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (-pivot);
        //Owner->SetHeight(Owner->RawSize.Y - v);
        Owner->SetMinHeight(tmp + v);
        break;
    case ERelationType::TopExt_Bottom:
        tmp = Owner->GetMinHeight();
        if (info.bPercent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (1 - pivot);
        //Owner->SetHeight(Owner->RawSize.Y - v);
        Owner->SetMinHeight(tmp + v);
        break;
    case ERelationType::BottomExt_Top:
        tmp = Owner->GetMinHeight();
        //if (info.bPercent)
        //    v = pos + (tmp + Owner->RawSize.Y - pos) * delta - (tmp + Owner->RawSize.Y);
        //else
        //    v = delta * (-pivot);
        //Owner->SetHeight(Owner->RawSize.Y + v);
        Owner->SetMinHeight(tmp);
        break;
    case ERelationType::BottomExt_Bottom:
        tmp = Owner->GetMinHeight();
        if (info.bPercent)
        {
           /* if (Owner == InTarget->GetParent())
            {
                if (Owner->bUnderConstruct)
                    Owner->SetHeight(pos + InTarget->GetHeight() - InTarget->GetHeight() * pivot +
                    (Owner->GetSize().Y - pos - InTarget->GetSize().Y + InTarget->GetSize().Y * pivot) * delta);
                else
                    Owner->SetHeight(pos + (Owner->RawSize.Y - pos) * delta);
            }
            else
            {
                v = pos + (tmp + Owner->RawSize.Y - pos) * delta - (tmp + Owner->RawSize.Y);
                Owner->SetHeight(Owner->RawSize.Y + v);
                Owner->SetMinHeight(tmp);
            }*/
        }
        else
        {
            //if (Owner == InTarget->GetParent())
            //{
            //    if (Owner->bUnderConstruct)
            //        Owner->SetHeight(Owner->GetSize().Y + (InTarget->GetHeight() - InTarget->GetSize().Y) * (1 - pivot));
            //    else
            //        Owner->SetHeight(Owner->RawSize.Y + delta * (1 - pivot));
            //}
            //else
            //{
            //    v = delta * (1 - pivot);
            //    Owner->SetHeight(Owner->RawSize.Y + v);
            //    Owner->SetMinHeight(tmp);
            //}
        }
        break;
    default:
        break;
    }
}

void FRelationItem::AddRefTarget(UFairyObject* InTarget)
{
    if (!InTarget)
    {
        return;
    }

    if (InTarget != Owner->GetParent())
    {
        PositionDelegateHandle = InTarget->OnPositionChanged().AddRaw(this, &FRelationItem::OnTargetXYChanged);
    }
    SizeDelegateHandle = InTarget->OnSizeChanged().AddRaw(this, &FRelationItem::OnTargetSizeChanged);

    TargetData.X = InTarget->GetPosition().X;
    TargetData.Y = InTarget->GetPosition().Y;
    TargetData.Z = InTarget->GetWidth();
    TargetData.W = InTarget->GetHeight();
}

void FRelationItem::ReleaseRefTarget()
{
    if (Target == nullptr)
    {
        return;
    }

    Target->OnPositionChanged().Remove(PositionDelegateHandle);
    Target->OnSizeChanged().Remove(SizeDelegateHandle);
}

void FRelationItem::OnTargetXYChanged()
{
    //if (Owner->Relations->Handling != nullptr || (Owner->Group.IsValid() && Owner->Group->Updating != 0))
    //{
    //    TargetData.X = Target->GetPosition().X;
    //    TargetData.Y = Target->GetPosition().Y;
    //    return;
    //}

    //Owner->Relations->Handling = Target.Get();

    FVector2D Pos = Owner->GetPosition();
    float dx = Target->GetPosition().X - TargetData.X;
    float dy = Target->GetPosition().Y - TargetData.Y;

    for (auto& it : Defs)
    {
        ApplyOnXYChanged(Target, it, dx, dy);
    }

    TargetData.X = Target->GetPosition().X;
    TargetData.Y = Target->GetPosition().Y;

    if (Pos != Owner->GetPosition())
    {
        FVector2D Delta = Owner->GetPosition() - Pos;

        Owner->UpdateGearFromRelations(1, Delta);

        if (Owner->HasParent())
        {
            const TArray<UTransition*>& arr = Owner->GetParent()->GetTransitions();
            for (auto& it : arr)
            {
                it->UpdateFromRelations(Owner->GetID(), Delta);
            }
        }
    }

    //Owner->Relations->Handling = nullptr;
}

void FRelationItem::OnTargetSizeChanged()
{
    //if (Owner->Relations->Handling != nullptr
    //    || (Owner->Group.IsValid() && Owner->Group->Updating != 0) )
    //{
    //    TargetData.Z = Target->GetWidth();
    //    TargetData.W = Target->GetHeight();
    //    return;
    //}
    //Owner->Relations->Handling = Target.Get();

    //FVector2D Pos = Owner->GetPosition();
    //FVector2D RawSize = Owner->RawSize;

    //for (auto& it : Defs)
    //{
    //    ApplyOnSizeChanged(Target.Get(), it);
    //}

    //TargetData.Z = Target->GetWidth();
    //TargetData.W = Target->GetHeight();

    //if (Pos != Owner->GetPosition())
    //{
    //    FVector2D Delta = Owner->GetPosition() - Pos;

    //    Owner->UpdateGearFromRelations(1, Delta);

    //    if (Owner->HasParent())
    //    {
    //        const TArray<UTransition*>& arr = Owner->GetParent()->GetTransitions();
    //        for (auto& it : arr)
    //        {
    //            it->UpdateFromRelations(Owner->GetID(), Delta);
    //        }
    //    }
    //}

    //if (RawSize != Owner->RawSize)
    //{
    //    FVector2D Delta = Owner->RawSize - RawSize;

    //    Owner->UpdateGearFromRelations(2, Delta);
    //}

    //Owner->Relations->Handling = nullptr;
}