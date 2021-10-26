#include "UI/Relation/RelationDef.h"
#include "UI/FairyObject.h"
#include "UI/Relation/RelationItem.h"


FRelationDef::FRelationDef()
	: OwnerRelationItem(nullptr),
	RelationType(ERelationType::Left_Left),
	bUsePercent(false)
{}

FRelationDef::~FRelationDef()
{
}

void FRelationDef::ApplyRelation()
{
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
		DefPtr = new FRelationDef_Pos_Left_Left();
		break;
	case ERelationType::Left_Center:
		DefPtr = new FRelationDef_Pos_Left_Center();
		break;
	case ERelationType::Left_Right:
		DefPtr = new FRelationDef_Pos_Left_Right();
		break;
	case ERelationType::Center_Center:
		DefPtr = new FRelationDef_Pos_Center_Center();
		break;
	case ERelationType::Right_Left:
		DefPtr = new FRelationDef_Pos_Right_Left();
		break;
	case ERelationType::Right_Center:
		DefPtr = new FRelationDef_Pos_Right_Center();
		break;
	case ERelationType::Right_Right:
		DefPtr = new FRelationDef_Pos_Right_Right();
		break;
	case ERelationType::Top_Top:
		DefPtr = new FRelationDef_Pos_Top_Top();
		break;
	case ERelationType::Top_Middle:
		DefPtr = new FRelationDef_Pos_Top_Middle();
		break;
	case ERelationType::Top_Bottom:
		DefPtr = new FRelationDef_Pos_Top_Bottom();
		break;
	case ERelationType::Bottom_Top:
		DefPtr = new FRelationDef_Pos_Bottom_Top();
		break;
	case ERelationType::Bottom_Middle:
		DefPtr = new FRelationDef_Pos_Bottom_Middle();
		break;
	case ERelationType::Bottom_Bottom:
		DefPtr = new FRelationDef_Pos_Bottom_Bottom();
		break;
	case ERelationType::Width:
		DefPtr = new FRelationDef_Width();
		break;
	case ERelationType::Height:
		DefPtr = new FRelationDef_Height();
		break;
	case ERelationType::LeftExt_Left:
		DefPtr = new FRelationDef_Ext_Left_Left();
		break;
	case ERelationType::LeftExt_Right:
		DefPtr = new FRelationDef_Ext_Left_Right();
		break;
	case ERelationType::RightExt_Left:
		DefPtr = new FRelationDef_Ext_Right_Left();
		break;
	case ERelationType::RightExt_Right:
		DefPtr = new FRelationDef_Ext_Right_Right();
		break;
	case ERelationType::TopExt_Top:
		DefPtr = new FRelationDef_Ext_Top_Top();
		break;
	case ERelationType::TopExt_Bottom:
		DefPtr = new FRelationDef_Ext_Top_Bottom();
		break;
	case ERelationType::BottomExt_Top:
		DefPtr = new FRelationDef_Ext_Bottom_Top();
		break;
	case ERelationType::BottomExt_Bottom:
		DefPtr = new FRelationDef_Ext_Bottom_Bottom();
		break;
	default:
		break;
	}
	DefPtr->OwnerRelationItem = InOwner;
	DefPtr->RelationType = InRelationType;
	DefPtr->bUsePercent = InbUsePercent;
	return DefPtr;
}

void FRelationDef::ApplyRelationPos(const float OwnerOldEdgePos, const float TargetOldEdgePos, const float TargetNewEdgePos)
{
	FVector2D FinalPos = OwnerRelationItem->GetOwnerCachePos();

	ERelation::PosDirection Direction = GetPosRelationDirection(RelationType);
	const FVector2D TargetNewSize = OwnerRelationItem->GetTargetSize();
	const FVector2D TargetOldSize = OwnerRelationItem->GetTargetCachedSize();

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

	if (OwnerRelationItem->IsRelatedToContainer())
	{
		const FVector2D TargetPos = OwnerRelationItem->GetTargetPos();
		if (Direction == ERelation::PosDirection::Horizontal)
		{
			FinalPos.X -= OwnerRelationItem->GetTarget()->GetPosition().X;
		}
		else
		{
			FinalPos.Y -= OwnerRelationItem->GetTarget()->GetPosition().Y;
		}
	}
	OwnerRelationItem->GetOwner()->SetPosition(FinalPos);
}

void FRelationDef_Pos_Left_Left::ApplyRelation()
{
	if (!OwnerRelationItem->IsRelatedToContainer())
	{
		ApplyRelationPos(OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetTargetCachedLeftPos(), OwnerRelationItem->GetTargetLeftPos());
	}
}

void FRelationDef_Pos_Left_Center::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetTargetCachedCenterPos(), OwnerRelationItem->GetTargetCenterPos());
}

void FRelationDef_Pos_Left_Right::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetTargetCachedRightPos(), OwnerRelationItem->GetTargetRightPos());
}

void FRelationDef_Pos_Center_Center::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerCenterPos(), OwnerRelationItem->GetTargetCachedCenterPos(), OwnerRelationItem->GetTargetCenterPos());
}

void FRelationDef_Pos_Right_Left::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetTargetCachedLeftPos(), OwnerRelationItem->GetTargetLeftPos());
}

void FRelationDef_Pos_Right_Center::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetTargetCachedCenterPos(), OwnerRelationItem->GetTargetCenterPos());
}

void FRelationDef_Pos_Right_Right::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetTargetCachedRightPos(), OwnerRelationItem->GetTargetRightPos());
}

void FRelationDef_Pos_Top_Top::ApplyRelation()
{
	if (!OwnerRelationItem->IsRelatedToContainer())
	{
		ApplyRelationPos(OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetTargetCachedTopPos(), OwnerRelationItem->GetTargetTopPos());
	}
}

void FRelationDef_Pos_Top_Middle::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetTargetCachedMiddlePos(), OwnerRelationItem->GetTargetMiddlePos());
}

void FRelationDef_Pos_Top_Bottom::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetTargetCachedBottomPos(), OwnerRelationItem->GetTargetBottomPos());
}

void FRelationDef_Pos_Middle_Middle::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerMiddlePos(), OwnerRelationItem->GetTargetCachedMiddlePos(), OwnerRelationItem->GetTargetMiddlePos());
}

void FRelationDef_Pos_Bottom_Top::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetTargetCachedTopPos(), OwnerRelationItem->GetTargetTopPos());
}

void FRelationDef_Pos_Bottom_Middle::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetTargetCachedMiddlePos(), OwnerRelationItem->GetTargetMiddlePos());
}

void FRelationDef_Pos_Bottom_Bottom::ApplyRelation()
{
	ApplyRelationPos(OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetTargetCachedBottomPos(), OwnerRelationItem->GetTargetBottomPos());
}

void FRelationDef_Width::ApplyRelation()
{
	FVector2D OwnerSize = OwnerRelationItem->GetOwnerSize();
	FVector2D TargetSize = OwnerRelationItem->GetTargetSize();
	FVector2D TargetCachedSize = OwnerRelationItem->GetTargetCachedSize();

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

	OwnerRelationItem->GetOwner()->SetSize(FinalSize);
}

void FRelationDef_Height::ApplyRelation()
{
	FVector2D OwnerSize = OwnerRelationItem->GetOwnerSize();
	FVector2D TargetSize = OwnerRelationItem->GetTargetSize();
	FVector2D TargetCachedSize = OwnerRelationItem->GetTargetCachedSize();

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

	OwnerRelationItem->GetOwner()->SetSize(FinalSize);
}

void FRelationDef::ApplyRelationExt(const float InOwnerExtEdgePos, const float InOwnerConstantEdgePos, const float InTargetOldEdgePos, const float InTargetNewEdgePos)
{
	ERelation::ExtDirection direction = GetExtensionDirection(RelationType);
	check(direction != ERelation::ExtDirection::NoExtDirection);

	const FVector2D OwnerSize = OwnerRelationItem->GetOwnerSize();
	const FVector2D OwnerPos = OwnerRelationItem->GetOwnerPos();
	const FVector2D OwnerAnchor = OwnerRelationItem->GetOwnerAnchor();
	const FVector2D OwnerScale = OwnerRelationItem->GetOwnerScale();

	const FVector2D TargetOldSize = OwnerRelationItem->GetTargetCachedSize();
	const FVector2D TargetNewSize = OwnerRelationItem->GetTargetSize();

	FVector2D NewSize = OwnerSize;
	FVector2D NewPos = OwnerPos;

	float TargetEdgeDeltaValue = 0;
	if (bUsePercent)
	{
		float OldDenominator = (direction == ERelation::ExtDirection::Width) ? TargetOldSize.X : TargetOldSize.Y;
		float NewDenominator = (direction == ERelation::ExtDirection::Width) ? TargetNewSize.X : TargetNewSize.Y;
		if (OldDenominator != 0)
		{
			float Ratio = (InTargetOldEdgePos - InOwnerExtEdgePos) / OldDenominator;
			float OwnerNewEdgePos = InTargetNewEdgePos - NewDenominator * Ratio;
			TargetEdgeDeltaValue = OwnerNewEdgePos - InOwnerExtEdgePos;
		}
	}
	else
	{
		TargetEdgeDeltaValue = InTargetNewEdgePos - InTargetOldEdgePos;
	}

	float OwnerExtEdgeNewPos = InOwnerExtEdgePos + TargetEdgeDeltaValue;
	float MinorPos = OwnerExtEdgeNewPos < InOwnerConstantEdgePos ? OwnerExtEdgeNewPos : InOwnerConstantEdgePos; // get owner object left or top edge position
	if (direction == ERelation::ExtDirection::Width)
	{
		NewSize.X = FMath::Abs(InOwnerConstantEdgePos - OwnerExtEdgeNewPos);
		NewPos.X = MinorPos + NewSize.X * OwnerScale.X * OwnerAnchor.X;
	}
	else
	{
		NewSize.Y = FMath::Abs(InOwnerConstantEdgePos - OwnerExtEdgeNewPos);
		NewPos.Y = MinorPos + NewSize.Y * OwnerScale.Y * OwnerAnchor.Y;
	}

	OwnerRelationItem->GetOwner()->SetPosition(NewPos);
	OwnerRelationItem->GetOwner()->SetSize(NewSize);
}

void FRelationDef_Ext_Left_Left::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetTargetCachedLeftPos(), OwnerRelationItem->GetTargetLeftPos());
}

void FRelationDef_Ext_Left_Right::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetTargetCachedRightPos(), OwnerRelationItem->GetTargetRightPos());
}

void FRelationDef_Ext_Right_Left::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetTargetCachedLeftPos(), OwnerRelationItem->GetTargetLeftPos());
}

void FRelationDef_Ext_Right_Right::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerRightPos(), OwnerRelationItem->GetOwnerLeftPos(), OwnerRelationItem->GetTargetCachedRightPos(), OwnerRelationItem->GetTargetRightPos());
}

void FRelationDef_Ext_Top_Top::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetTargetCachedTopPos(), OwnerRelationItem->GetTargetTopPos());
}

void FRelationDef_Ext_Top_Bottom::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetTargetCachedBottomPos(), OwnerRelationItem->GetTargetBottomPos());
}

void FRelationDef_Ext_Bottom_Top::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetTargetCachedTopPos(), OwnerRelationItem->GetTargetTopPos());
}

void FRelationDef_Ext_Bottom_Bottom::ApplyRelation()
{
	ApplyRelationExt(OwnerRelationItem->GetOwnerBottomPos(), OwnerRelationItem->GetOwnerTopPos(), OwnerRelationItem->GetTargetCachedBottomPos(), OwnerRelationItem->GetTargetBottomPos());
}

