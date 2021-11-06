// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/TransitionItem.h"
#include "UI/FieldTypes.h"

FTransitionItem::FTransitionItem(ETransitionActionType InType) :
	Time(0),
	Type(InType),
	Tweener(nullptr),
	Target(nullptr),
	DisplayLockToken(0)
{
	switch (InType)
	{
	case ETransitionActionType::XY:
	case ETransitionActionType::Size:
	case ETransitionActionType::Scale:
	case ETransitionActionType::Pivot:
	case ETransitionActionType::Skew:
	case ETransitionActionType::Alpha:
	case ETransitionActionType::Rotation:
	case ETransitionActionType::Color:
	case ETransitionActionType::ColorFilter:
		Data.Emplace();
		break;

	case ETransitionActionType::Animation:
		AniData.Emplace();
		break;

	case ETransitionActionType::Shake:
		ShakeData.Emplace();
		break;

	case ETransitionActionType::Sound:
		SoundData.Emplace();
		break;

	case ETransitionActionType::Transition:
		TransData.Emplace();
		break;

	case ETransitionActionType::Visible:
		VisibleData.Emplace();
		break;

	case ETransitionActionType::Text:
	case ETransitionActionType::Icon:
		TextData.Emplace();
		break;

	default:
		break;
	}
}

FTransitionItem::~FTransitionItem()
{
	if (Tweener != nullptr)
	{
		Tweener->Kill();
	}
}
