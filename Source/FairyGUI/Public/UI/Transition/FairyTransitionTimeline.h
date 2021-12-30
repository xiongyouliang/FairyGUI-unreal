// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FFairyTransitionItemBase;
class UFairyObject;
class UFairyTransition;
/**
 * 
 */
class FAIRYGUI_API FFairyTransitionTimeline
{
private:
	EFairyTransitionItemType TimelineType;
	TWeakObjectPtr<UFairyObject> TargetObjectPtr;
	TWeakObjectPtr<UFairyTransition> OwnerObjectPtr;

	TArray<TSharedPtr<FFairyTransitionItemBase>> ItemList;

public:
	void SetTarget(UFairyObject* InTargetObject);
	UFairyObject* GetTarget();

	void SetOwner(UFairyTransition* InOwnerObject);
	UFairyTransition* GetOwner();

	void SetTimelineType(EFairyTransitionItemType InType);
	EFairyTransitionItemType GetTimelineType();

	/**
	* repeat times value means:
	* -1: repeat forever;
	* 0: play once;
	* >1: play times for give value;
	*/
	int32 GetRepeatTimes();

	void AddTransitionItem(FFairyTransitionItemBase* InTransitionItem);

	void Play();
};
