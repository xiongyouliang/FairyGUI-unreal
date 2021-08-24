#pragma once

#include "CoreMinimal.h"
#include "UI/FieldTypes.h"

class FRelationItem;

class FRelationDef
{
public:
	FRelationItem* OwnerRelationItem;
	ERelationType RelationType;
	bool bUsePercent;

	FRelationDef();

	virtual ~FRelationDef();
	virtual void ApplyRelation();

	void ApplyRelationPos(const float OwnerOldEdgePos, const float TargetOldEdgePos, const float TargetNewEdgePos);
	void ApplyRelationExt(const float InOwnerExtEdgePos, const float InOwnerConstantEdgePos, const float InTargetOldEdgePos, const float InTargetNewEdgePos);
};

FRelationDef* CreateRelationDef(FRelationItem* InOwner, ERelationType InRelationType, bool bUserPercent);
ERelation::PosDirection GetPosRelationDirection(ERelationType RelationType);
ERelation::ExtDirection GetExtensionDirection(ERelationType RelationType);

class FRelationDef_Pos_Left_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Left_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Left_Right : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Center_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Right_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Right_Center : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Right_Right : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Top_Top: public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Top_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Top_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Middle_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Bottom_Top : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Bottom_Middle : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Pos_Bottom_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Width : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Height : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Left_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Left_Right : public FRelationDef
{
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Right_Left : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Right_Right : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Top_Top : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Top_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Bottom_Top : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};

class FRelationDef_Ext_Bottom_Bottom : public FRelationDef
{
public:
	virtual void ApplyRelation() override;
};
