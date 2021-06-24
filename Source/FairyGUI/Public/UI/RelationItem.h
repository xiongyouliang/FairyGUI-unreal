#pragma once

#include "CoreMinimal.h"
#include "FieldTypes.h"

class UFairyObject;

struct FAIRYGUI_API FRelationDef
{
    bool bPercent;
    ERelationType Type;
    int32 Axis;

    FRelationDef() :
        bPercent(false), Type(ERelationType::Left_Left), Axis(0)
    {}
};

class FAIRYGUI_API FRelationItem
{
public:
    FRelationItem(UFairyObject* InOwner);
    ~FRelationItem();

    UFairyObject* GetTarget() const { return Target.Get(); }
    void SetTarget(UFairyObject* InTarget);

    void Add(ERelationType RelationType, bool bUsePercent);
    void InternalAdd(ERelationType RelationType, bool bUsePercent);
    void Remove(ERelationType RelationType);
    void CopyFrom(const FRelationItem& Source);
    bool IsEmpty() const;
    void ApplyOnSelfSizeChanged(float DeltaWidth, float DeltaHeight, bool bApplyPivot);

private:
    void ApplyOnXYChanged(UFairyObject* InTarget, const FRelationDef& info, float dx, float dy);
    void ApplyOnSizeChanged(UFairyObject* InTarget, const FRelationDef& info);
    void AddRefTarget(UFairyObject* InTarget);
    void ReleaseRefTarget();
    void OnTargetXYChanged();
    void OnTargetSizeChanged();

    UFairyObject* Owner;
    TWeakObjectPtr<UFairyObject> Target;
    TArray<FRelationDef> Defs;
    FVector4 TargetData;

    FDelegateHandle PositionDelegateHandle;
    FDelegateHandle SizeDelegateHandle;
};