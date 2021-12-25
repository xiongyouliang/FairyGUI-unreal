#include "UI/Relation/RelationItem.h"
#include "UI/Relation/Relations.h"
#include "UI/Relation/RelationDef.h"

#include "UI/FairyObject.h"
#include "UI/FairyComponent.h"
#include "UI/GGroup.h"
#include "UI/Transition/FairyTransition.h"

FRelationItem::FRelationItem(UFairyObject* InOwner) :
	Owner(nullptr),
	Target(nullptr),
	TargetCachePos(FVector2D::ZeroVector),
	TargetCacheSize(FVector2D::ZeroVector),
	TargetCacheAnchor(FVector2D::ZeroVector)
{
	Owner = InOwner;
}

FRelationItem::~FRelationItem()
{
}

UFairyObject* FRelationItem::GetTarget() const
{
	return Target;
}

void FRelationItem::SetTarget(UFairyObject* InTarget)
{
	if (InTarget && Target != InTarget)
	{
		Target = InTarget;
		CacheRelationValue();
	}
}

FVector2D FRelationItem::GetTargetSize()
{
	return Target->GetSize();
}

FVector2D FRelationItem::GetTargetPos()
{
	return Target->GetPosition();
}

FVector2D FRelationItem::GetTargetAnchor()
{
	return Target->GetAnchor();
}

FVector2D FRelationItem::GetTargetCachedSize()
{
	return TargetCacheSize;
}

FVector2D FRelationItem::GetTargetCachedPos()
{
	return TargetCachePos;
}

UFairyObject* FRelationItem::GetOwner()
{
	return Owner;
}

FVector2D FRelationItem::GetOwnerSize()
{
	return Owner->GetSize();
}

FVector2D FRelationItem::GetOwnerPos()
{
	FVector2D OwnerPos = Owner->GetPosition();
	return OwnerPos;
}

FVector2D FRelationItem::GetOwnerPosInTargetSpace()
{
	FVector2D Pos = Owner->GetPosition();
	if (IsRelatedToContainer())
	{
		Pos += Target->GetPosition();
	}
	return Pos;
}

FVector2D FRelationItem::GetOwnerCachePos()
{
	return OwnerCachePos;
}

FVector2D FRelationItem::GetOwnerAnchor()
{
	return Owner->GetAnchor();
}

void FRelationItem::CacheRelationValue()
{
	if (Owner && Target)
	{
		TargetCachePos = Target->GetPosition();
		TargetCacheSize = Target->GetSize();
		TargetCacheAnchor = Target->GetAnchor();
		TargetCacheScale = Target->GetScale();

		OwnerCachePos = GetOwnerPosInTargetSpace();
	}
}

FVector2D FRelationItem::GetOwnerScale()
{
	return Owner->GetScale();
}

void FRelationItem::Add(ERelationType RelationType, bool bUsePercent)
{
	if (RelationType == ERelationType::Size)
	{
		Add(ERelationType::Width, bUsePercent);
		Add(ERelationType::Height, bUsePercent);
		return;
	}
	// filter same Relation type and conflict type
	// conflict: only support one position relation and one extension relation in single direction
	for (auto it : Defs)
	{
		// same relation type
		if (it->RelationType == RelationType)
		{
			return;
		}

		// just support only one position relation type
		if (RelationType >= ERelationType::Left_Left && RelationType <= ERelationType::Bottom_Bottom && GetPosRelationDirection(RelationType) == GetPosRelationDirection(it->RelationType))
		{
			return;
		}

		// just support only one extension relation type
		if (RelationType >= ERelationType::Width && RelationType < ERelationType::Size && GetExtensionDirection(RelationType) == GetExtensionDirection(it->RelationType))
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

	FRelationDef* info = CreateRelationDef(this, RelationType, bUsePercent);
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
		if (def->RelationType == RelationType)
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

bool FRelationItem::IsRelatedToContainer()
{
	if (Target == Owner->GetParent())
	{
		return true;
	}

	return false;
}

void FRelationItem::ApplyRelation()
{
	const FVector2D TargetSize = Target->GetSize(); // if target has relation from other object
	const FVector2D TargetPos = Target->GetPosition();
	const FVector2D TargetAnchor = Target->GetAnchor();

	if (TargetCacheSize != TargetSize || TargetCachePos != TargetPos || TargetCacheAnchor != TargetAnchor)
	{
		for (auto& it : Defs)
		{
			it->ApplyRelation();
		}

		CacheRelationValue();
	}
}

// *********************** Owner object edge position start ********************
float FRelationItem::GetOwnerLeftPos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = GetOwnerPosInTargetSpace();
	const FVector2D OwnerAnchor = Owner->GetAnchor();
	const FVector2D OwnerScale = Owner->GetScale();
	return OwnerPos.X - OwnerSize.X * OwnerAnchor.X * OwnerScale.X;
}

float FRelationItem::GetOwnerCenterPos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	const FVector2D OwnerScale = Owner->GetScale();
	return GetOwnerLeftPos() + OwnerSize.X * OwnerScale.X * 0.5f;
}

float FRelationItem::GetOwnerRightPos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	const FVector2D OwnerScale = Owner->GetScale();
	return GetOwnerLeftPos() + OwnerSize.X * OwnerScale.X;
}

float FRelationItem::GetOwnerTopPos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = Owner->GetPosition();
	if (IsRelatedToContainer())
	{
		OwnerPos += Target->GetPosition();
	}
	const FVector2D OwnerAnchor = Owner->GetAnchor();
	const FVector2D OwnerScale = Owner->GetScale();
	return OwnerPos.Y - OwnerSize.Y * OwnerAnchor.Y * OwnerScale.Y;
}

float FRelationItem::GetOwnerMiddlePos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	const FVector2D OwnerScale = Owner->GetScale();
	return GetOwnerTopPos() + OwnerSize.Y * OwnerScale.Y * 0.5f;
}

float FRelationItem::GetOwnerBottomPos()
{
	const FVector2D OwnerSize = Owner->GetSize();
	const FVector2D OwnerScale = Owner->GetScale();
	return GetOwnerTopPos() + OwnerSize.Y * OwnerScale.Y;
}
// *********************** Owner object edge position end ********************

// *********************** Target object edge position and cached position start ********************
float FRelationItem::GetTargetLeftPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	const FVector2D& TargetScale = Target->GetScale();
	return TargetPos.X - TargetSize.X * TargetAnchor.X;
}

float FRelationItem::GetTargetCenterPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetScale = Target->GetScale();
	return GetTargetLeftPos() + TargetSize.X * TargetScale.X * 0.5f;
}

float FRelationItem::GetTargetRightPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetScale = Target->GetScale();
	return GetTargetLeftPos() + TargetSize.X * TargetScale.X;
}

float FRelationItem::GetTargetTopPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	const FVector2D& TargetScale = Target->GetScale();
	return TargetPos.Y - TargetSize.Y * TargetAnchor.Y * TargetScale.Y;
}

float FRelationItem::GetTargetMiddlePos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetScale = Target->GetScale();
	return GetTargetTopPos() + TargetSize.Y * TargetScale.Y * 0.5f;
}

float FRelationItem::GetTargetBottomPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetScale = Target->GetScale();
	return GetTargetTopPos() + TargetSize.Y * TargetScale.Y;
}

float FRelationItem::GetTargetCachedLeftPos()
{
	return TargetCachePos.X - TargetCacheSize.X * TargetCacheScale.X * TargetCacheAnchor.X;
}

float FRelationItem::GetTargetCachedCenterPos()
{
	return GetTargetCachedLeftPos() + TargetCacheSize.X * TargetCacheScale.X * 0.5;
}

float FRelationItem::GetTargetCachedRightPos()
{
	return GetTargetCachedLeftPos() + TargetCacheSize.X * TargetCacheScale.X;
}

float FRelationItem::GetTargetCachedTopPos()
{
	return TargetCachePos.Y - TargetCacheSize.Y * TargetCacheScale.Y * TargetCacheAnchor.Y;
}

float FRelationItem::GetTargetCachedMiddlePos()
{
	return GetTargetCachedTopPos() + TargetCacheSize.Y * TargetCacheScale.Y * 0.5;
}

float FRelationItem::GetTargetCachedBottomPos()
{
	return GetTargetCachedTopPos() + TargetCacheSize.Y * TargetCacheScale.Y;
}

// *********************** Target object edge position start ********************