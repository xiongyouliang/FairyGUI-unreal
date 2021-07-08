#pragma once

#include "CoreMinimal.h"
#include "FieldTypes.h"
#include "Math/Axis.h"

class UFairyObject;

struct FAIRYGUI_API FRelationDef
{
    bool bPercent;
    ERelationType Type;
    EAxis::Type Axis;

    FRelationDef() :
        bPercent(false), Type(ERelationType::Left_Left), Axis(EAxis::Type::X)
    {}
};

class FAIRYGUI_API FRelationItem
{
public:
    FRelationItem(UFairyObject* InOwner);
    ~FRelationItem();

    UFairyObject* GetTarget() const { return Target; }
    void SetTarget(UFairyObject* InTarget);

    void Add(ERelationType RelationType, bool bUsePercent);
    void InternalAdd(ERelationType RelationType, bool bUsePercent);
    void Remove(ERelationType RelationType);
    void CopyFrom(const FRelationItem& Source);
    bool IsEmpty() const;
    void ApplySelfSizeChanged(float DeltaWidth, float DeltaHeight, bool bApplyPivot);

    FVector2D GetRelationSize();
    FVector2D GetRelationPos();

private:
    void ApplyTargetPositionChanged(const FRelationDef& info);
    void ApplyTargetSizeChanged(const FRelationDef& info);
    void BindTarget();
    void ReleaseRefTarget();
    void OnTargetPositionChanged();
    void OnTargetSizeChanged();

    UFairyObject* Owner;
    UFairyObject* Target;
    TArray<FRelationDef> Defs;
    FVector4 TargetData;

    FDelegateHandle PositionDelegateHandle;
    FDelegateHandle SizeDelegateHandle;
};