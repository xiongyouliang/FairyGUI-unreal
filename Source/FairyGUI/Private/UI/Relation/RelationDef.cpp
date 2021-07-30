#include "UI/Relation/RelationDef.h"
#include "UI/FairyObject.h"
#include "UI/Relation/RelationItem.h"


FRelationDef::FRelationDef()
	: Owner(nullptr),
	RelationType(ERelationType::Left_Left),
	bUsePercent(false)
{}

FRelationDef::~FRelationDef()
{
}

void FRelationDef::ApplyRelation()
{
}

void FRelationDef::ApplyRelationPos(const float OwnerOldEdgePos, const float TargetOldEdgePos, const float TargetNewEdgePos)
{
	FVector2D FinalPos = Owner->GetOwnerPos();

	ERelation::PosDirection Direction = GetPosRelationDirection(RelationType);
	const FVector2D TargetNewSize = Owner->GetTargetSize();
	const FVector2D TargetOldSize = Owner->GetTargetCachedSize();

	if (bUsePercent)
	{
		if (Direction == ERelation::PosDirection::Horizontal)
		{
			if (TargetOldSize.X != 0)
			{
				float Ratio = (TargetOldEdgePos - OwnerOldEdgePos) / TargetOldSize.X;
				FinalPos.X += (TargetNewEdgePos - Ratio * TargetNewSize.X - OwnerOldEdgePos);
			}
		}
		else
		{
			if (TargetOldSize.Y != 0)
			{
				float Ratio = (TargetOldEdgePos - OwnerOldEdgePos) / TargetOldSize.Y;
				FinalPos.Y += (TargetNewEdgePos - Ratio * TargetNewSize.Y - OwnerOldEdgePos);
			}
		}
	}
	else
	{
		if (Direction == ERelation::PosDirection::Horizontal)
		{
			FinalPos.X += (TargetNewEdgePos - TargetOldEdgePos);
		}
		else
		{
			FinalPos.Y += (TargetNewEdgePos - TargetOldEdgePos);
		}
	}
	Owner->GetOwner()->SetPosition(FinalPos);
}

ERelation::PosDirection GetPosRelationDirection(ERelationType RelationType)
{
	if (RelationType >= ERelationType::Left_Left && RelationType <= ERelationType::Right_Right)
	{
		return ERelation::PosDirection::Horizontal;
	}

	if (RelationType >= ERelationType::Top_Top && RelationType <= ERelationType::Bottom_Bottom)
	{
		return ERelation::PosDirection::Vertical;
	}

	return ERelation::PosDirection::NoPosDirection;
}

ERelation::ExtDirection GetExtensionDirection(ERelationType RelationType)
{
	if (RelationType >= ERelationType::LeftExt_Left && RelationType <= ERelationType::RightExt_Right)
	{
		return ERelation::ExtDirection::Width;
	}

	if (RelationType == ERelationType::Width)
	{
		return ERelation::ExtDirection::Width;
	}

	if (RelationType >= ERelationType::TopExt_Top && RelationType <= ERelationType::BottomExt_Bottom)
	{
		return ERelation::ExtDirection::Height;
	}

	if (RelationType == ERelationType::Height)
	{
		return ERelation::ExtDirection::Height;
	}

	return ERelation::ExtDirection::NoExtDirection;
}

FRelationDef* CreateRelationDef(FRelationItem* InOwner, ERelationType InRelationType, bool InbUsePercent)
{
	FRelationDef* DefPtr = nullptr;
	switch (InRelationType)
	{
	case ERelationType::Left_Left:
		DefPtr = new FRelationDef_Left_Left();
		break;
	case ERelationType::Left_Center:
		DefPtr = new FRelationDef_Left_Center();
		break;
	case ERelationType::Left_Right:
		DefPtr = new FRelationDef_Left_Right();
		break;
	case ERelationType::Center_Center:
		DefPtr = new FRelationDef_Center_Center();
		break;
	case ERelationType::Right_Left:
		DefPtr = new FRelationDef_Right_Left();
		break;
	case ERelationType::Right_Center:
		DefPtr = new FRelationDef_Right_Center();
		break;
	case ERelationType::Right_Right:
		DefPtr = new FRelationDef_Right_Right();
		break;
	case ERelationType::Top_Top:
		DefPtr = new FRelationDef_Top_Top();
		break;
	case ERelationType::Top_Middle:
		DefPtr = new FRelationDef_Top_Middle();
		break;
	case ERelationType::Top_Bottom:
		DefPtr = new FRelationDef_Top_Bottom();
		break;
	case ERelationType::Bottom_Top:
		DefPtr = new FRelationDef_Bottom_Top();
		break;
	case ERelationType::Bottom_Middle:
		DefPtr = new FRelationDef_Bottom_Middle();
		break;
	case ERelationType::Bottom_Bottom:
		DefPtr = new FRelationDef_Bottom_Bottom();
		break;
	case ERelationType::Width:
		DefPtr = new FRelationDef_Width();
		break;
	case ERelationType::Height:
		DefPtr = new FRelationDef_Height();
		break;
	default:
		break;
	}
	DefPtr->Owner = InOwner;
	DefPtr->RelationType = InRelationType;
	DefPtr->bUsePercent = InbUsePercent;
	return DefPtr;
}

void FRelationDef_Left_Left::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerLeftPos(), Owner->GetTargetCachedLeftPos(), Owner->GetTargetLeftPos());
}

void FRelationDef_Left_Center::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerLeftPos(), Owner->GetTargetCachedCenterPos(), Owner->GetTargetCenterPos());
}

void FRelationDef_Left_Right::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerLeftPos(), Owner->GetTargetCachedRightPos(), Owner->GetTargetRightPos());
}

void FRelationDef_Center_Center::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerCenterPos(), Owner->GetTargetCachedCenterPos(), Owner->GetTargetCenterPos());
}

void FRelationDef_Right_Left::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerRightPos(), Owner->GetTargetCachedLeftPos(), Owner->GetTargetLeftPos());
}

void FRelationDef_Right_Center::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerRightPos(), Owner->GetTargetCachedCenterPos(), Owner->GetTargetCenterPos());
}

void FRelationDef_Right_Right::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerRightPos(), Owner->GetTargetCachedRightPos(), Owner->GetTargetRightPos());
}

void FRelationDef_Top_Top::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerTopPos(), Owner->GetTargetCachedTopPos(), Owner->GetTargetTopPos());
}

void FRelationDef_Top_Middle::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerTopPos(), Owner->GetTargetCachedMiddlePos(), Owner->GetTargetMiddlePos());
}

void FRelationDef_Top_Bottom::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerTopPos(), Owner->GetTargetCachedBottomPos(), Owner->GetTargetBottomPos());
}

void FRelationDef_Middle_Middle::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerMiddlePos(), Owner->GetTargetCachedMiddlePos(), Owner->GetTargetMiddlePos());
}

void FRelationDef_Bottom_Top::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerBottomPos(), Owner->GetTargetCachedTopPos(), Owner->GetTargetTopPos());
}

void FRelationDef_Bottom_Middle::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerBottomPos(), Owner->GetTargetCachedMiddlePos(), Owner->GetTargetMiddlePos());
}

void FRelationDef_Bottom_Bottom::ApplyRelation()
{
	ApplyRelationPos(Owner->GetOwnerBottomPos(), Owner->GetTargetCachedBottomPos(), Owner->GetTargetBottomPos());
}

void FRelationDef_Width::ApplyRelation()
{
	FVector2D OwnerSize = Owner->GetOwnerSize();
	FVector2D TargetSize = Owner->GetTargetSize();
	FVector2D TargetCachedSize = Owner->GetTargetCachedSize();

	FVector2D FinalSize = OwnerSize;
	if (bUsePercent)
	{
		if (TargetCachedSize.X != 0)
		{
			float WidthRatio = OwnerSize.X / TargetCachedSize.X;
			FinalSize.X = TargetSize.X * WidthRatio;
		}
	}
	else
	{
		float deltaValue = TargetSize.X - TargetCachedSize.X;
		FinalSize.X = OwnerSize.X + deltaValue;
	}

	Owner->GetOwner()->SetSize(FinalSize);
}

void FRelationDef_Height::ApplyRelation()
{
	FVector2D OwnerSize = Owner->GetOwnerSize();
	FVector2D TargetSize = Owner->GetTargetSize();
	FVector2D TargetCachedSize = Owner->GetTargetCachedSize();

	FVector2D FinalSize = OwnerSize;
	if (bUsePercent)
	{
		if (TargetCachedSize.Y != 0)
		{
			float HeightRatio = OwnerSize.Y / TargetCachedSize.Y;
			FinalSize.Y = TargetSize.Y * HeightRatio;
		}
	}
	else
	{
		float deltaValue = TargetSize.Y - TargetCachedSize.Y;
		FinalSize.Y = OwnerSize.Y + deltaValue;
	}

	Owner->GetOwner()->SetSize(FinalSize);
}
