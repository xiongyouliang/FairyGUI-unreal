#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RelationItem.h"

class UFairyObject;
class FByteBuffer;

class FAIRYGUI_API FRelations
{
public:
    FRelations(UFairyObject* InOwner);
    ~FRelations();

    void Add(UFairyObject* InTarget, ERelationType RelationType);
    void Add(UFairyObject* InTarget, ERelationType RelationType, bool bUsePercent);
    void Remove(UFairyObject* InTarget, ERelationType RelationType);
    bool Contains(UFairyObject* InTarget);
    void ClearFor(UFairyObject* InTarget);
    void ClearAll();
    void CopyFrom(const FRelations& Source);
    void OnOwnerSizeChanged(const FVector2D& Delta, bool bApplyPivot);
    bool IsEmpty() const;
    void Setup(FByteBuffer* Buffer, bool bParentToChild);

    UFairyObject* Handling;

private:
    UFairyObject* Owner;
    TIndirectArray<FRelationItem> Items;
};