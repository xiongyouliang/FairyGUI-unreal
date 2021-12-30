#include "Tween/FairyTweenMgr.h"

#include "FairyApplication.h"

void UFairyTweenerArrayWrapper::RemoveTweener(UFairyTweener* InTweener)
{
	for (size_t i = 0; i < TweenerList.Num(); i++)
	{
		if (InTweener == TweenerList[i])
		{
			TweenerList.RemoveAt(i);
			break;
		}
	}
}

UFairyTweenMgr* UFairyTweenMgr::Instance = nullptr;

UFairyTweenMgr::UFairyTweenMgr()
	:bTicking(false)
{
}

UFairyTweenMgr::~UFairyTweenMgr()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTweenMananger Destructed!"));
	}
}

UFairyTweenerPos* UFairyTweenMgr::CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	UFairyTweenerPos* Tweener = NewObject<UFairyTweenerPos>(this);
	Tweener->Init(InDuration, InStartPos, InDstPos);
	return Tweener;
}

UFairyTweenerSize* UFairyTweenMgr::CreateTweenerSize(float InDuration, FVector2D InStartSize, FVector2D InDstSize)
{
	UFairyTweenerSize* Tweener = NewObject<UFairyTweenerSize>(this);
	Tweener->Init(InDuration, InStartSize, InDstSize);
	return Tweener;
}

UFairyTweenerOpacity* UFairyTweenMgr::CreateTweenerOpacity(float InDuration, float InStartOpacity, float InDstOpacity)
{
	UFairyTweenerOpacity* Tweener = NewObject<UFairyTweenerOpacity>(this);
	Tweener->Init(InDuration, InStartOpacity, InDstOpacity);
	return Tweener;
}

UFairyTweenerRotate* UFairyTweenMgr::CreateTweenerRotate(float InDuration, float InStartRotate, float InDstRotate)
{
	UFairyTweenerRotate* Tweener = NewObject<UFairyTweenerRotate>(this);
	Tweener->Init(InDuration, InStartRotate, InDstRotate);
	return Tweener;
}

UFairyTweenerScale* UFairyTweenMgr::CreateTweenerScale(float InDuration, FVector2D InStartScale, FVector2D InDstScale)
{
	UFairyTweenerScale* Tweener = NewObject<UFairyTweenerScale>(this);
	Tweener->Init(InDuration, InStartScale, InDstScale);
	return Tweener;
}

UFairyTweenerSkew* UFairyTweenMgr::CreateTweenerSkew(float InDuration, FVector2D InStartSkew, FVector2D InDstSkew)
{
	UFairyTweenerSkew* Tweener = NewObject<UFairyTweenerSkew>(this);
	Tweener->Init(InDuration, InStartSkew, InDstSkew);
	return Tweener;
}

UFairyTweenerColor* UFairyTweenMgr::CreateTweenerColor(float InDuration, FColor InStartColor, FColor InDstColor)
{
	UFairyTweenerColor* Tweener = NewObject<UFairyTweenerColor>(this);
	Tweener->Init(InDuration, InStartColor, InDstColor);
	return Tweener;
}

UFairyTweenerSequence* UFairyTweenMgr::CreateTweenerSequence(const TArray<UFairyTweenerFiniteTime*>& InTweenerList)
{
	UFairyTweenerSequence* Tweener = NewObject<UFairyTweenerSequence>(this);
	Tweener->Init(InTweenerList);

	return Tweener;
}

UFairyTweenerRepeat* UFairyTweenMgr::CreateTweenerRepeat(UFairyTweenerFiniteTime* InTweener, int32 InTimes)
{
	UFairyTweenerRepeat* Tweener = NewObject<UFairyTweenerRepeat>(this);
	Tweener->Init(InTweener, InTimes);

	return Tweener;
}

UFairyTweenerDelay* UFairyTweenMgr::CreateTweenerDelay(float InDelayTime)
{
	UFairyTweenerDelay* Tweener = NewObject<UFairyTweenerDelay>(this);
	Tweener->Init(InDelayTime);
	return Tweener;
}

UFairyTweenerShow* UFairyTweenMgr::CreateTweenerShow()
{
	return NewObject<UFairyTweenerShow>(this);
}

UFairyTweenerHide* UFairyTweenMgr::CreateTweenerHide()
{
	return NewObject<UFairyTweenerHide>(this);
}

UFairyTweenerPivot* UFairyTweenMgr::CreateTweenerPivot(FVector2D InDstPivot)
{
	UFairyTweenerPivot* Tweener = NewObject<UFairyTweenerPivot>(this);
	Tweener->Init(InDstPivot);
	return Tweener;
}

UFairyTweenerShake* UFairyTweenMgr::CreateTweenerShake(float InDuration, float Amplitude)
{
	UFairyTweenerShake* Tweener = NewObject<UFairyTweenerShake>(this);
	Tweener->Init(InDuration, Amplitude);
	return Tweener;
}

UFairyTweenerCallFunc* UFairyTweenMgr::CreateTweenerCallFunc()
{
	UFairyTweenerCallFunc* Tweener = NewObject<UFairyTweenerCallFunc>(this);
	Tweener->Init();
	return Tweener;
}

UFairyTweenerDynamicCallFunc* UFairyTweenMgr::CreateTweenerDynamicCallFunc(const FTweenDynamicDelegate& InDelegate)
{
	UFairyTweenerDynamicCallFunc* Tweener = NewObject<UFairyTweenerDynamicCallFunc>(this);
	Tweener->Init(InDelegate);
	return Tweener;
}

UFairyTweenerEase* UFairyTweenMgr::CreateTweenerEase(UFairyTweenerInterval* InInnerTweener, EFairyEaseType InEaseType, float InArgs)
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


void UFairyTweenMgr::AddTweener(UFairyTweener* InTweener, UFairyObject* InTarget, bool InPaused)
{
	if (InTweener)
	{
		if (!TweenerTable.Contains(InTarget))
		{
			TweenerTable.Add(InTarget, NewObject<UFairyTweenerArrayWrapper>(this));
		}
		UFairyTweenerArrayWrapper* Array = TweenerTable[InTarget];
		InTweener->SetPaused(InPaused);
		InTweener->SetTarget(InTarget);
		Array->AddTweener(InTweener);
	}
}

UFairyTweener* UFairyTweenMgr::GetTweenerByTag(int InTag, UFairyObject* InTarget)
{
	UFairyTweener* target = nullptr;
	if (TweenerTable.Contains(InTarget))
	{
		UFairyTweenerArrayWrapper* Array = TweenerTable[InTarget];
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

void UFairyTweenMgr::RemoveTweener(UFairyTweener* InTweener)
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

void UFairyTweenMgr::RemoveTweenerByTag(int InTag, UFairyObject* InTarget)
{
	if (TweenerTable.Contains(InTarget))
	{
		UFairyTweenerArrayWrapper* Array = TweenerTable[InTarget];
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

void UFairyTweenMgr::RemoveAllTweenerWithTarget(UFairyObject* InTarget)
{
	if (TweenerTable.Contains(InTarget))
	{
		UFairyTweenerArrayWrapper* Array = TweenerTable[InTarget];
		for (size_t i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			RemoveTweener(element);
		}
	}
}

void UFairyTweenMgr::DoRemoveTweener(UFairyTweener* InTweener)
{
	TArray<UFairyObject*> PendingDeleteArray;
	for (auto& Pair : TweenerTable)
	{
		UFairyTweenerArrayWrapper* Array = Pair.Value;
		for (int i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			if (element == InTweener)
			{
				Array->RemoveTweener(element);
				break;
			}
		}
		// If tweener array is empty, remove it from TweenerTable
		if (Array->Num() == 0)
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
	//InTweener->MarkPendingKill();
}

void UFairyTweenMgr::Tick(float DeltaTime)
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
		UFairyTweenerArrayWrapper* Array = Pair.Value;
		for (size_t i = 0; i < Array->Num(); i++)
		{
			UFairyTweener* element = (*Array)[i];
			if (!element->IsPaused() && element->IsTargetValid())
			{
				element->Step(DeltaTime);
				if (element->IsDone())
				{
					RemoveTweener(element);
				}
			}
		}
	}

	bTicking = false;
}
