#include "Tween/TweenManager.h"

#include "FairyApplication.h"

UTweenManager* UTweenManager::Instance = nullptr;

UTweenManager* UTweenManager::Get()
{
	if (UTweenManager::Instance == nullptr)
	{
		UTweenManager::Instance = NewObject<UTweenManager>(UFairyApplication::Get());
		
		// todo: maybe make this object as class UFairyApplication singletion object's property is better.
		UTweenManager::Instance->AddToRoot(); 
	}
	return UTweenManager::Instance;
}

UTweenManager::UTweenManager()
	:bTicking(false)
{
}

UTweenManager::~UTweenManager()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTweenMananger Destructed!"));
	}
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

UFairyTweenerEase* UTweenManager::CreateTweenerEase(UFairyTweenerInterval* InInnerTweener, EFairyEaseType InEaseType, float InArgs)
{
	UFairyTweenerEaseElastic* ElasticTweener = nullptr;
	UFairyTweenerEaseRate* RateTweeener = nullptr;
	UFairyTweenerEase* Tweener = nullptr;
	switch (InEaseType)
	{
	case EFairyEaseType::Linear:
		Tweener = NewObject<UFairyTweenerEase>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::SineIn:
		Tweener = NewObject<UFairyTweenerEaseSineIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::SineOut:
		Tweener = NewObject<UFairyTweenerEaseSineOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::SineInOut:
		Tweener = NewObject<UFairyTweenerEaseSineInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuadIn:
		Tweener = NewObject<UFairyTweenerEaseQuadIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuadOut:
		Tweener = NewObject<UFairyTweenerEaseQuadOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuadInOut:
		Tweener = NewObject<UFairyTweenerEaseQuadInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CubicIn:
		Tweener = NewObject<UFairyTweenerEaseCubicIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CubicOut:
		Tweener = NewObject<UFairyTweenerEaseCubicInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CubicInOut:
		Tweener = NewObject<UFairyTweenerEaseCubicInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuartIn:
		Tweener = NewObject<UFairyTweenerEaseQuartIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuartOut:
		Tweener = NewObject<UFairyTweenerEaseQuartOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuartInOut:
		Tweener = NewObject<UFairyTweenerEaseQuadInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuintIn:
		Tweener = NewObject<UFairyTweenerEaseQuintIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuintOut:
		Tweener = NewObject<UFairyTweenerEaseQuintOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::QuintInOut:
		Tweener = NewObject<UFairyTweenerEaseQuintInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::ExpoIn:
		Tweener = NewObject<UFairyTweenerEaseExpoIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::ExpoOut:
		Tweener = NewObject<UFairyTweenerEaseExpoOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::ExpoInOut:
		Tweener = NewObject<UFairyTweenerEaseExpoInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CircIn:
		Tweener = NewObject<UFairyTweenerEaseCircIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CircOut:
		Tweener = NewObject<UFairyTweenerEaseCircOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::CircInOut:
		Tweener = NewObject<UFairyTweenerEaseCircInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::ElasticIn:
		ElasticTweener = NewObject<UFairyTweenerEaseElasticIn>(this);
		ElasticTweener->Init(InInnerTweener, InArgs);
		Tweener = ElasticTweener;
		break;
	case EFairyEaseType::ElasticOut:
		ElasticTweener = NewObject<UFairyTweenerEaseElasticOut>(this);
		ElasticTweener->Init(InInnerTweener, InArgs);
		Tweener = ElasticTweener;
		break;
	case EFairyEaseType::ElasticInOut:
		ElasticTweener = NewObject<UFairyTweenerEaseElasticInOut>(this);
		ElasticTweener->Init(InInnerTweener, InArgs);
		Tweener = ElasticTweener;
		break;
	case EFairyEaseType::BackIn:
		Tweener = NewObject<UFairyTweenerEaseBackIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::BackOut:
		Tweener = NewObject<UFairyTweenerEaseBackOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::BackInOut:
		Tweener = NewObject<UFairyTweenerEaseBackInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::BounceIn:
		Tweener = NewObject<UFairyTweenerEaseBounceIn>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::BounceOut:
		Tweener = NewObject<UFairyTweenerEaseBounceOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::BounceInOut:
		Tweener = NewObject<UFairyTweenerEaseBounceInOut>(this);
		Tweener->Init(InInnerTweener);
		break;
	case EFairyEaseType::Custom:
		// todo:
		break;
	case EFairyEaseType::RateIn:
		RateTweeener = NewObject<UFairyTweenerEaseRateIn>(this);
		RateTweeener->Init(InInnerTweener, InArgs);
		Tweener = RateTweeener;
		break;
	case EFairyEaseType::RateOut:
		RateTweeener = NewObject<UFairyTweenerEaseRateOut>(this);
		RateTweeener->Init(InInnerTweener, InArgs);
		Tweener = RateTweeener;
		break;
	case EFairyEaseType::RateInOut:
		RateTweeener = NewObject<UFairyTweenerEaseRateOut>(this);
		RateTweeener->Init(InInnerTweener, InArgs);
		Tweener = RateTweeener;
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
		PrePendingRemoveArray.Add(InTweener);
	}
	else
	{
		DoRemoveTweener(InTweener);
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
			}
		}
	}
}

void UTweenManager::RemoveAllTweenerWithTarget(UFairyObject* InTarget)
{
	if (TweenerTable.Contains(InTarget))
	{
		TArray<UFairyTweener*>* Array = TweenerTable.Find(InTarget);
		for (size_t i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			RemoveTweener(element);
		}
	}
}

void UTweenManager::DoRemoveTweener(UFairyTweener* InTweener)
{
	TArray<UFairyObject*> PendingDeleteArray;
	for (auto& Pair : TweenerTable)
	{
		TArray<UFairyTweener*>& Array = Pair.Value;
		for (int i = 0; i < Array.Num(); i++)
		{
			const UFairyTweener* element = Array[i];
			if (element == InTweener)
			{
				Array.RemoveAt(i);
				break;
			}
		}
		// If tweener array is empty, remove it from TweenerTable
		if (Array.Num() == 0)
		{
			PendingDeleteArray.Add(Pair.Key);
		}
	}

	for (size_t i = 0; i < PendingDeleteArray.Num(); i++)
	{
		const UFairyObject* Target = PendingDeleteArray[i];
		if (TweenerTable.Contains(Target))
		{
			TweenerTable.Remove(Target);
		}
	}

	InTweener->SetTarget(nullptr);
	InTweener->MarkPendingKill();
}

void UTweenManager::Tick(float DeltaTime)
{
	for (size_t i = 0; i < PrePendingRemoveArray.Num(); i++)
	{
		UFairyTweener* element = PrePendingRemoveArray[i];
		DoRemoveTweener(element);
	}
	PrePendingRemoveArray.Reset();

	bTicking = true;
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

	bTicking = false;
}
