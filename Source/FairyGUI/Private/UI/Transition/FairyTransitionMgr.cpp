// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FairyTransitionMgr.h"
#include "FairyCommons.h"

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