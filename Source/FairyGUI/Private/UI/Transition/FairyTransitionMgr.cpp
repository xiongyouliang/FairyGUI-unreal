// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionMgr.h"
#include "UI/Transition/FairyTransition.h"
#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/Transition/FairyTransitionItemPos.h"
#include "UI/Transition/FairyTransitionItemSize.h"
#include "UI/Transition/FairyTransitionItemAlpha.h"
#include "UI/Transition/FairyTransitionItemRotation.h"
#include "UI/Transition/FairyTransitionItemScale.h"
#include "UI/Transition/FairyTransitionItemSkew.h"
#include "UI/Transition/FairyTransitionItemColor.h"
#include "UI/Transition/FairyTransitionItemPivot.h"
#include "UI/Transition/FairyTransitionItemVisible.h"
#include "UI/Transition/FairyTransitionItemSound.h"
#include "UI/Transition/FairyTransitionItemShake.h"
#include "FairyCommons.h"
#include "UI/FieldTypes.h"

void UFairyTransitionMgr::AddTransition(UFairyTransition* InTransition)
{
	if (!TransitionList.Contains(InTransition))
	{
		TransitionList.Add(InTransition);
	}
	else
	{
		UE_LOG(LogFairyGUI, Warning, TEXT("The pendding add transition is exit."));
	}
}

void UFairyTransitionMgr::RemoveTransition(UFairyTransition* InTransition)
{
	if (TransitionList.Contains(InTransition))
	{
		TransitionList.Remove(InTransition);
	}
	else
	{
		UE_LOG(LogFairyGUI, Warning, TEXT("The pendding remove transition is not exit."));
	}
}

void UFairyTransitionMgr::RemoveAllTransition()
{
	
}

void UFairyTransitionMgr::OnComponentEnter(UFairyComponent* InTargetComponent)
{
	for (size_t i = 0; i < TransitionList.Num(); i++)
	{
		UFairyTransition* Transition = TransitionList[i];
		if (InTargetComponent == Transition->GetTargetComponent())
		{
			Transition->OnTargetComponentEnter();
		}
	}
}

void UFairyTransitionMgr::OnComponentExit(UFairyComponent* InTargetComponent)
{
	for (size_t i = 0; i < TransitionList.Num(); i++)
	{
		UFairyTransition* Transition = TransitionList[i];
		if (InTargetComponent == Transition->GetTargetComponent())
		{
			Transition->OnTargetComponentExit();
			RemoveTransition(Transition);
		}
	}
}

int32 UFairyTransitionMgr::GetTransitionNum()
{
	return TransitionList.Num();
}

int32 UFairyTransitionMgr::GetTransitionNumInComponent(UFairyComponent* InTargetComponent)
{
	int32 num = 0;

	for (size_t i = 0; i < TransitionList.Num(); i++)
	{
		if (TransitionList[i]->GetTargetComponent() == InTargetComponent)
		{
			num++;
		}
	}

	return num;
}

FFairyTransitionItemBase* CreateTransitionItemWithType(const EFairyTransitionItemType InActionType)
{
	FFairyTransitionItemBase* item = nullptr;
	switch (InActionType)
	{
	case EFairyTransitionItemType::XY:
		item = new FFairyTransitionItemPos();
		break;
	case EFairyTransitionItemType::Size:
		item = new FFairyTransitionItemSize();
		break;
	case EFairyTransitionItemType::Scale:
		item = new FFairyTransitionItemScale();
		break;
	case EFairyTransitionItemType::Pivot:
		item = new FFairyTransitionItemPivot();
		break;
	case EFairyTransitionItemType::Alpha:
		item = new FFairyTransitionItemAlpha();
		break;
	case EFairyTransitionItemType::Rotation:
		item = new FFairyTransitionItemRotation();
		break;
	case EFairyTransitionItemType::Color:
		item = new FFairyTransitionItemColor();
		break;
	case EFairyTransitionItemType::Animation:
		break;
	case EFairyTransitionItemType::Visible:
		item = new FFairyTransitionItemVisible();
		break;
	case EFairyTransitionItemType::Sound:
		item = new FFairyTransitionItemSound();
		break;
	case EFairyTransitionItemType::Transition:
		break;
	case EFairyTransitionItemType::Shake:
		item = new FFairyTransitionItemShake();
		break;
	case EFairyTransitionItemType::ColorFilter:
		break;
	case EFairyTransitionItemType::Skew:
		item = new FFairyTransitionItemSkew();
		break;
	case EFairyTransitionItemType::Text:
		break;
	case EFairyTransitionItemType::Icon:
		break;
	case EFairyTransitionItemType::Unknown:
		break;
	default:
		break;
	}

	return item;
}
