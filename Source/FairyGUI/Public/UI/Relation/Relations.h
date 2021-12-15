#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Relation/RelationItem.h"
#include "Utils/ByteBuffer.h"

class UFairyObject;

/**
* A UFairyObject Handle a FRelations Object;
* A FRelations Handle a list FRelationItem Objects;
* A FRelationItem Handle a Target UFairyObject;
* What happend when the Target UFairyObject is destoried?
*/
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
	bool IsEmpty() const;
	void Setup(FairyGUI::FByteBuffer* Buffer, bool bParentToChild);

	void ApplyRelation() const;

	UFairyObject* Handling;

private:
	UFairyObject* Owner;
	TArray<FRelationItem*> ItemList;
};