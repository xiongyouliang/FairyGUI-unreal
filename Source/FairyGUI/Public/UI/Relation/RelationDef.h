#pragma once

#include "CoreMinimal.h"
#include "UI/FieldTypes.h"

class FRelationItem;

class FAIRYGUI_API FRelationDef
{
public:
	FRelationItem* Owner;
	ERelationType RelationType;
	bool bUsePercent;

	FRelationDef();

	virtual ~FRelationDef();
	virtual void ApplyRelation();

	void ApplyRelationPos(const float OwnerOldEdgePos, const float TargetOldEdgePos, const float TargetNewEdgePos);
};

FRelationDef* CreateRelationDef(FRelationItem* InOwner, ERelationType InRelationType, bool bUserPercent);
ERelation::PosDirection GetPosRelationDirection(ERelationType RelationType);
ERelation::ExtDirection GetExtensionDirection(ERelationType RelationType);

class FAIRYGUI_API FRelationDef_Left_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Left_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Left_Right : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Center_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Right_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Right_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Right_Right : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Top_Top: public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Top_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Top_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Middle_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Bottom_Top : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Bottom_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Bottom_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Width : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FAIRYGUI_API FRelationDef_Height : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

