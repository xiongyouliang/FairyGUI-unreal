#include "UI/Relation/RelationItem.h"
#include "UI/Relation/Relations.h"
#include "UI/Relation/RelationDef.h"

#include "UI/FairyObject.h"
#include "UI/FairyComponent.h"
#include "UI/GGroup.h"
#include "UI/Transition.h"

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
	return Owner->GetPosition();
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

float FRelationItem::GetOwnerLeftPos()
{
	FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = Owner->GetPosition();
	FVector2D OwnerAnchor = Owner->GetAnchor();
	return OwnerPos.X - OwnerSize.X * OwnerAnchor.X;
}

float FRelationItem::GetOwnerCenterPos()
{
	return GetOwnerLeftPos() + Owner->GetSize().X * 0.5f;
}

float FRelationItem::GetOwnerRightPos()
{
	FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = Owner->GetPosition();
	FVector2D OwnerAnchor = Owner->GetAnchor();
	return OwnerPos.X + OwnerSize.X * OwnerAnchor.X;
}

float FRelationItem::GetOwnerTopPos()
{
	FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = Owner->GetPosition();
	FVector2D OwnerAnchor = Owner->GetAnchor();
	return OwnerPos.Y - OwnerSize.Y * OwnerAnchor.Y;
}

float FRelationItem::GetOwnerMiddlePos()
{
	return GetOwnerTopPos() + Owner->GetSize().Y * 0.5f;
}

float FRelationItem::GetOwnerBottomPos()
{
	FVector2D OwnerSize = Owner->GetSize();
	FVector2D OwnerPos = Owner->GetPosition();
	FVector2D OwnerAnchor = Owner->GetAnchor();
	return OwnerPos.Y + OwnerSize.Y * OwnerAnchor.Y;
}

float FRelationItem::GetTargetLeftPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	return TargetPos.X - TargetSize.X * TargetAnchor.X;
}

float FRelationItem::GetTargetCenterPos()
{
	return GetTargetLeftPos() + Target->GetSize().X * 0.5f;
}

float FRelationItem::GetTargetRightPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	return TargetPos.X + TargetSize.X * TargetAnchor.X;
}

float FRelationItem::GetTargetTopPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	return TargetPos.Y - TargetSize.Y * TargetAnchor.Y;
}

float FRelationItem::GetTargetMiddlePos()
{
	return GetTargetTopPos() + Target->GetSize().Y * 0.5f;
}

float FRelationItem::GetTargetBottomPos()
{
	const FVector2D& TargetSize = Target->GetSize();
	const FVector2D& TargetPos = Target->GetPosition();
	const FVector2D& TargetAnchor = Target->GetAnchor();
	return TargetPos.Y + TargetSize.Y * TargetAnchor.Y;
}

float FRelationItem::GetTargetCachedLeftPos()
{
	return TargetCachePos.X - TargetCacheSize.X * TargetCacheAnchor.X;
}

float FRelationItem::GetTargetCachedCenterPos()
{
	return GetTargetCachedLeftPos() + TargetCacheSize.X * 0.5;
}

float FRelationItem::GetTargetCachedRightPos()
{
	return TargetCachePos.X + TargetCacheSize.X * TargetCacheAnchor.X;
}

float FRelationItem::GetTargetCachedTopPos()
{
	return TargetCachePos.Y - TargetCacheSize.Y * TargetCacheAnchor.Y;
}

float FRelationItem::GetTargetCachedMiddlePos()
{
	return GetTargetCachedTopPos() + TargetCacheSize.Y * 0.5;
}

float FRelationItem::GetTargetCachedBottomPos()
{
	return TargetCachePos.Y + TargetCacheSize.Y * TargetCacheAnchor.Y;
}