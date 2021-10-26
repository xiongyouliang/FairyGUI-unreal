#pragma once

#include "CoreMinimal.h"
#include "UI/FieldTypes.h"
#include "Math/Axis.h"

class FRelationDef;
class UFariyObject;

class FAIRYGUI_API FRelationItem
{
public:
	FRelationItem(UFairyObject* InOwner);
	~FRelationItem();

	UFairyObject* GetTarget() const;
	void SetTarget(UFairyObject* InTarget);
	FVector2D GetTargetSize();
	FVector2D GetTargetPos();
	FVector2D GetTargetAnchor();
	FVector2D GetTargetCachedSize();
	FVector2D GetTargetCachedPos();

	UFairyObject* GetOwner();
	inline FVector2D GetOwnerSize();
	inline FVector2D GetOwnerPos();
	inline FVector2D GetOwnerPosInTargetSpace();
	inline FVector2D GetOwnerCachePos();
	inline FVector2D GetOwnerAnchor();
	inline FVector2D GetOwnerScale();

	void Add(ERelationType RelationType, bool bUsePercent);
	void InternalAdd(ERelationType RelationType, bool bUsePercent);
	void Remove(ERelationType RelationType);
	void CopyFrom(const FRelationItem& Source);
	bool IsEmpty() const;
	bool IsRelatedToContainer();

	/**
	* Extension Owner Object's Left/Right/Top/Bottom edge(this means size, position both changed)
	*/
	inline float GetOwnerLeftPos();
	inline float GetOwnerCenterPos();
	inline float GetOwnerRightPos();
	inline float GetOwnerTopPos();
	inline float GetOwnerMiddlePos();
	inline float GetOwnerBottomPos();

	inline float GetTargetLeftPos();
	inline float GetTargetCenterPos();
	inline float GetTargetRightPos();
	inline float GetTargetTopPos();
	inline float GetTargetMiddlePos();
	inline float GetTargetBottomPos();

	inline float GetTargetCachedLeftPos();
	inline float GetTargetCachedCenterPos();
	inline float GetTargetCachedRightPos();
	inline float GetTargetCachedTopPos();
	inline float GetTargetCachedMiddlePos();
	inline float GetTargetCachedBottomPos();

	void ApplyRelation();

protected:
	void CacheRelationValue();

	UFairyObject* Owner; // Element
	UFairyObject* Target; // Container
	TArray<FRelationDef*> Defs;

	/// OwnerCachePos and TargetCachePos in the same coordination space
	FVector2D OwnerCachePos;
	FVector2D TargetCachePos;
	FVector2D TargetCacheSize;
	FVector2D TargetCacheScale;
	FVector2D TargetCacheAnchor;
};