#include "Tween/TweenManager.h"

#include "FairyApplication.h"

UTweenManager* UTweenManager::Instance = nullptr;

UTweenManager* UTweenManager::Get()
{
	if (UTweenManager::Instance == nullptr)
	{
		UTweenManager::Instance = NewObject<UTweenManager>(UFairyApplication::Get());
	}
	return UTweenManager::Instance;
}

UTweenManager::UTweenManager()
	:bTicking(false)
{
}

UTweenManager::~UTweenManager()
{
}

UFairyTweenerPos* UTweenManager::CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	UFairyTweenerPos* Tweener = NewObject<UFairyTweenerPos>(this);
	Tweener->Init(InDuration, InStartPos, InDstPos);
	return Tweener;
}

UFairyTweenerSize* UTweenManager::CreateTweenerSize(float InDuration, FVector2D InStartSize, FVector2D InDstSize)
{
	UFairyTweenerSize* Tweener = NewObject<UFairyTweenerSize>(this);
	Tweener->Init(InDuration, InStartSize, InDstSize);
	return Tweener;
}

UFairyTweenerOpacity* UTweenManager::CreateTweenerOpacity(float InDuration, float InStartOpacity, float InDstOpacity)
{
	UFairyTweenerOpacity* Tweener = NewObject<UFairyTweenerOpacity>(this);
	Tweener->Init(InDuration, InStartOpacity, InDstOpacity);
	return Tweener;
}

UFairyTweenerRotate* UTweenManager::CreateTweenerRotate(float InDuration, float InStartRotate, float InDstRotate)
{
	UFairyTweenerRotate* Tweener = NewObject<UFairyTweenerRotate>(this);
	Tweener->Init(InDuration, InStartRotate, InDstRotate);
	return Tweener;
}

UFairyTweenerScale* UTweenManager::CreateTweenerScale(float InDuration, FVector2D InStartScale, FVector2D InDstScale)
{
	UFairyTweenerScale* Tweener = NewObject<UFairyTweenerScale>(this);
	Tweener->Init(InDuration, InStartScale, InDstScale);
	return Tweener;
}

UFairyTweenerSkew* UTweenManager::CreateTweenerSkew(float InDuration, FVector2D InStartSkew, FVector2D InDstSkew)
{
	UFairyTweenerSkew* Tweener = NewObject<UFairyTweenerSkew>(this);
	Tweener->Init(InDuration, InStartSkew, InDstSkew);
	return Tweener;
}

UFairyTweenerColor* UTweenManager::CreateTweenerColor(float InDuration, FColor InStartColor, FColor InDstColor)
{
	UFairyTweenerColor* Tweener = NewObject<UFairyTweenerColor>(this);
	Tweener->Init(InDuration, InStartColor, InDstColor);
	return Tweener;
}

UFairyTweenerSequence* UTweenManager::CreateTweenerSequence(const TArray<UFairyTweenerFiniteTime*>& InTweenerList)
{
	UFairyTweenerSequence* Tweener = NewObject<UFairyTweenerSequence>(this);
	Tweener->Init(InTweenerList);

	return Tweener;
}

UFairyTweenerDelay* UTweenManager::CreateTweenerDelay(float InDelayTime)
{
	UFairyTweenerDelay* Tweener = NewObject<UFairyTweenerDelay>(this);
	Tweener->Init(InDelayTime);
	return Tweener;
}

UFairyTweenerShow* UTweenManager::CreateTweenerShow()
{
	return NewObject<UFairyTweenerShow>(this);
}

UFairyTweenerHide* UTweenManager::CreateTweenerHide()
{
	return NewObject<UFairyTweenerHide>(this);
}

UFairyTweenerPivot* UTweenManager::CreateTweenerPivot(FVector2D InDstPivot)
{
	UFairyTweenerPivot* Tweener = NewObject<UFairyTweenerPivot>(this);
	Tweener->Init(InDstPivot);
	return Tweener;
}

UFairyTweenerCallFunc* UTweenManager::CreateTweenerCallFunc(const FTweenDelegate& InDelegate)
{
	UFairyTweenerCallFunc* Tweener = NewObject<UFairyTweenerCallFunc>(this);
	Tweener->Init(InDelegate);
	return Tweener;
}

UFairyTweenerDynamicCallFunc* UTweenManager::CreateTweenerDynamicCallFunc(const FTweenDynamicDelegate& InDelegate)
{
	UFairyTweenerDynamicCallFunc* Tweener = NewObject<UFairyTweenerDynamicCallFunc>(this);
	Tweener->Init(InDelegate);
	return Tweener;
}

UFairyTweenerEase* UTweenManager::CreateTweenerEase(UFairyTweenerInterval* InInnerTweener, EEaseType InEaseType, float InArgs)
{
	UFairyTweenerEase* Tweener = nullptr;
	switch (InEaseType)
	{
	case EEaseType::Linear:
		Tweener = NewObject<UFairyTweenerEase>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::SineIn:
		Tweener = NewObject<UFairyTweenerEaseSineIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::SineOut:
		Tweener = NewObject<UFairyTweenerEaseSineOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::SineInOut:
		Tweener = NewObject<UFairyTweenerEaseSineInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuadIn:
		Tweener = NewObject<UFairyTweenerEaseQuadIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuadOut:
		Tweener = NewObject<UFairyTweenerEaseQuadOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuadInOut:
		Tweener = NewObject<UFairyTweenerEaseQuadInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CubicIn:
		Tweener = NewObject<UFairyTweenerEaseCubicIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CubicOut:
		Tweener = NewObject<UFairyTweenerEaseCubicInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CubicInOut:
		Tweener = NewObject<UFairyTweenerEaseCubicInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuartIn:
		Tweener = NewObject<UFairyTweenerEaseQuartIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuartOut:
		Tweener = NewObject<UFairyTweenerEaseQuartOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuartInOut:
		Tweener = NewObject<UFairyTweenerEaseQuadInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuintIn:
		Tweener = NewObject<UFairyTweenerEaseQuintIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuintOut:
		Tweener = NewObject<UFairyTweenerEaseQuintOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::QuintInOut:
		Tweener = NewObject<UFairyTweenerEaseQuintInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::ExpoIn:
		Tweener = NewObject<UFairyTweenerEaseExpoIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::ExpoOut:
		Tweener = NewObject<UFairyTweenerEaseExpoOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::ExpoInOut:
		Tweener = NewObject<UFairyTweenerEaseExpoInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CircIn:
		Tweener = NewObject<UFairyTweenerEaseCircIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CircOut:
		Tweener = NewObject<UFairyTweenerEaseCircOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::CircInOut:
		Tweener = NewObject<UFairyTweenerEaseCircInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::ElasticIn:
		UFairyTweenerEaseElastic* TempTweener = NewObject<UFairyTweenerEaseElasticIn>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	case EEaseType::ElasticOut:
		UFairyTweenerEaseElastic* TempTweener = NewObject<UFairyTweenerEaseElasticOut>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	case EEaseType::ElasticInOut:
		UFairyTweenerEaseElastic* TempTweener = NewObject<UFairyTweenerEaseElasticInOut>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	case EEaseType::BackIn:
		Tweener = NewObject<UFairyTweenerEaseBackIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::BackOut:
		Tweener = NewObject<UFairyTweenerEaseBackOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::BackInOut:
		Tweener = NewObject<UFairyTweenerEaseBackInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::BounceIn:
		Tweener = NewObject<UFairyTweenerEaseBounceIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::BounceOut:
		Tweener = NewObject<UFairyTweenerEaseBounceOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::BounceInOut:
		Tweener = NewObject<UFairyTweenerEaseBounceInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EEaseType::Custom:
		// todo:
		break;
	case EEaseType::RateIn:
		UFairyTweenerEaseRateIn* TempTweener = NewObject<UFairyTweenerEaseRateIn>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	case EEaseType::RateOut:
		UFairyTweenerEaseRateOut* TempTweener = NewObject<UFairyTweenerEaseRateOut>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	case EEaseType::RateInOut:
		UFairyTweenerEaseRateOut* TempTweener = NewObject<UFairyTweenerEaseRateOut>(this);
		TempTweener->Init(InInnerTweener, InArgs);
		Tweener = TempTweener;
		break;
	default:
		Tweener = NewObject<UFairyTweenerEaseQuadIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	}
	
	return Tweener;
}


void UTweenManager::AddTweener(UFairyTweener* InTweener, UFairyObject* InTarget, bool InPaused)
{
	if (InTweener)
	{
		TArray<UFairyTweener*>& Array = TweenerTable.FindOrAdd(InTarget);
		InTweener->SetPaused(InPaused);
		InTweener->StartWithTarget(InTarget);
		Array.Add(InTweener);
	}
}

UFairyTweener* UTweenManager::GetTweenerByTag(int InTag, UFairyObject* InTarget)
{
	UFairyTweener* target = nullptr;
	if (TweenerTable.Contains(InTarget))
	{
		TArray<UFairyTweener*>* Array = TweenerTable.Find(InTarget);
		for (size_t i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			if (element->GetTag() == InTag)
			{
				target = element;
				break;
			}
		}
	}
	return target;
}

void UTweenManager::RemoveTweener(UFairyTweener* InTweener)
{
	if (!InTweener)
	{
		return;
	}

	if (bTicking)
	{
		PostTickRemoveArray.Add(InTweener);
	}
	else
	{
		PreTickRemoveArray.Add(InTweener);
	}
}

void UTweenManager::RemoveTweenerByTag(int InTag, UFairyObject* InTarget)
{
	if (TweenerTable.Contains(InTarget))
	{
		TArray<UFairyTweener*>* Array = TweenerTable.Find(InTarget);
		for (size_t i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			if (element->GetTag() == InTag)
			{
				RemoveTweener(element);
				break;
			}
		}
	}
}

void UTweenManager::RemoveAllTweenerWithTarget(UFairyObject* InTarget)
{
	if (TweenerTable.Contains(InTarget))
	{
		if (bTicking == false)
		{
			// quick remove
			TweenerTable.Remove(InTarget);
		}
		else
		{
			TArray<UFairyTweener*>* Array = TweenerTable.Find(InTarget);
			for (size_t i = 0; i < Array->Num(); i++)
			{
				UFairyTweener* element = (*Array)[i];
				PostTickRemoveArray.Add(element);
			}
		}
	}
}

void UTweenManager::DoRemoveTweener(UFairyTweener* InTweener)
{
	TArray<UFairyObject*> DeleteArray;
	for (auto& Pair : TweenerTable)
	{
		TArray<UFairyTweener*>& Array = Pair.Value;
		for (int i = 0; i < Array.Num(); i++)
		{
			UFairyTweener* element = Array[i];
			if (element == InTweener)
			{
				Array.RemoveAt(i);
				break;
			}
		}
		// If tweener array is empty, remove it from TweenerTable
		if (Array.Num() == 0)
		{
			DeleteArray.Add(Pair.Key);
		}
	}

	for (size_t i = 0; i < DeleteArray.Num(); i++)
	{
		RemoveAllTweenerWithTarget(DeleteArray[i]);
	}
}

void UTweenManager::Tick(float DeltaTime)
{
	bTicking = true;

	for (size_t i = 0; i < PreTickRemoveArray.Num(); i++)
	{
		UFairyTweener* element = PreTickRemoveArray[i];
		DoRemoveTweener(element);
	}
	PreTickRemoveArray.Reset();


	for (auto& Pair : TweenerTable)
	{
		TArray<UFairyTweener*>& Array = Pair.Value;
		for (size_t i = 0; i < Array.Num(); i++)
		{
			UFairyTweener* element = Array[i];
			if (!element->IsPaused())
			{
				if (element->IsDone() || !element->IsTargetValid())
				{
					RemoveTweener(element);
					continue;
				}
				else
				{
					element->Step(DeltaTime);
				}
			}
		}
	}

	for (size_t i = 0; i < PostTickRemoveArray.Num(); i++)
	{
		UFairyTweener* element = PostTickRemoveArray[i];
		DoRemoveTweener(element);
	}
	PostTickRemoveArray.Reset();

	bTicking = false;
}
