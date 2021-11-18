#include "Tween/TweenManager.h"
#include "Tween/FairyTweener.h"
#include "Tween/FairyTweenerInterval.h"

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
	TotalActiveTweenerNum = 0;
	ActiveTweenerPointerCapcity = 30;
	ActiveTweenerPointerArray = new FairyTweenerPointer[ActiveTweenerPointerCapcity]();
}

UTweenManager::~UTweenManager()
{
	for (auto it : TweenerPool)
	{
		delete it;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr)
		{
			delete tweener;
		}
	}
	delete []ActiveTweenerPointerArray;
}

UFairyTweener* UTweenManager::CreateTweener()
{
	UFairyTweener* tweener = nullptr;
	int32 cnt = TweenerPool.Num();
	if (cnt > 0)
	{
		tweener = TweenerPool.Pop();
		tweener->Handle.IncreaseSerialNumber();
	}
	else
	{
		TweenerInstanceCount++;
		if (!ensureMsgf(TweenerInstanceCount != FTweenerHandle::MaxIndex, TEXT("Tweener index number has wrapped around!")))
		{
			TweenerInstanceCount = 0;
		}
		tweener = NewObject<UFairyTweener>(this);
		tweener->Handle.SetIndex(TweenerInstanceCount);
	}
	tweener->Init();
	ActiveTweenerPointerArray[TotalActiveTweenerNum++] = tweener;

	if (TotalActiveTweenerNum == ActiveTweenerPointerCapcity)
	{
		int32 newCapcity = ActiveTweenerPointerCapcity + FMath::CeilToInt(ActiveTweenerPointerCapcity * 0.5f);
		UFairyTweener** newArray = new FairyTweenerPointer[newCapcity];
		FMemory::Memcpy(newArray, ActiveTweenerPointerArray, ActiveTweenerPointerCapcity * sizeof(UFairyTweener*));
		delete []ActiveTweenerPointerArray;
		ActiveTweenerPointerArray = newArray;
		ActiveTweenerPointerCapcity = newCapcity;
	}

	return tweener;
}

UFairyTweenerPos* UTweenManager::CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos)
{
	UFairyTweenerPos* Tweener = NewObject<UFairyTweenerPos>(this);
	Tweener->Init(InDuration, InStartPos, InDstPos);
	return Tweener;
}

void UTweenManager::AddTweener(UFairyTweener* InTweener, UFairyObject* InTarget, bool InPaused)
{
	if (InTweener)
	{
		TArray<UFairyTweener*>& Array = TweenerTable.FindOrAdd(InTarget);
		InTweener->bPaused = InPaused;
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

bool UTweenManager::KillTween(FTweenerHandle & Handle, bool bCompleted)
{
	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Handle == Handle && !tweener->bKilled)
		{
			Handle.Invalidate();
			tweener->Kill(bCompleted);
			return true;
		}
	}

	Handle.Invalidate();
	return false;
}

bool UTweenManager::KillTweens(UObject* Target, bool bCompleted)
{
	if (Target == nullptr)
	{
		return false;
	}

	bool result = false;
	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Target.Get() == Target && !tweener->bKilled)
		{
			tweener->Kill(bCompleted);
			result = true;
		}
	}

	return result;
}

UFairyTweener* UTweenManager::GetTween(FTweenerHandle const& Handle)
{
	if (!Handle.IsValid())
	{
		return nullptr;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Handle == Handle && !tweener->bKilled)
		{
			return tweener;
		}
	}

	return nullptr;
}

UFairyTweener* UTweenManager::GetTween(UObject* Target)
{
	if (Target == nullptr)
	{
		return nullptr;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		UFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Target.Get() == Target && !tweener->bKilled)
		{
			return tweener;
		}
	}

	return nullptr;
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