#include "Tween/TweenManager.h"
#include "Tween/FairyTweener.h"

FTweenManager::FTweenManager()
{
	TotalActiveTweenerNum = 0;
	ActiveTweenerPointerCapcity = 30;
	ActiveTweenerPointerArray = new FairyTweenerPointer[ActiveTweenerPointerCapcity]();
}

FTweenManager::~FTweenManager()
{
	for (auto it : TweenerPool)
	{
		delete it;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		FFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr)
		{
			delete tweener;
		}
	}
	delete []ActiveTweenerPointerArray;
}

FFairyTweener* FTweenManager::CreateTweener()
{
	FFairyTweener* tweener = nullptr;
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
		tweener = new FFairyTweener();
		tweener->Handle.SetIndex(TweenerInstanceCount);
	}
	tweener->Init();
	ActiveTweenerPointerArray[TotalActiveTweenerNum++] = tweener;

	if (TotalActiveTweenerNum == ActiveTweenerPointerCapcity)
	{
		int32 newCapcity = ActiveTweenerPointerCapcity + FMath::CeilToInt(ActiveTweenerPointerCapcity * 0.5f);
		FFairyTweener** newArray = new FairyTweenerPointer[newCapcity];
		FMemory::Memcpy(newArray, ActiveTweenerPointerArray, ActiveTweenerPointerCapcity * sizeof(FFairyTweener*));
		delete []ActiveTweenerPointerArray;
		ActiveTweenerPointerArray = newArray;
		ActiveTweenerPointerCapcity = newCapcity;
	}

	return tweener;
}

void FTweenManager::AddTweener(TSharedPtr<FFairyTweener> &InTweener, UFairyObject* InTarget, bool InPaused)
{
	if (InTweener.IsValid())
	{
		TweenerArray& Array = TweenerTable.FindOrAdd(TweenerTableKey(InTarget));
		InTweener->bPaused = InPaused;
		Array.Add(InTweener);
	}
}

void FTweenManager::RemoveTweener(TSharedPtr<FFairyTweener> &InTweener)
{
	for (auto& Pair : TweenerTable)
	{
		TweenerArray& Array = Pair.Value;
		for (int i = 0; i < Array.Num(); i++)
		{
			TSharedPtr<FFairyTweener>& Element = Array[i];
			if (Element.Get() == InTweener.Get())
			{
				Array.RemoveAt(i);
				break;
			}
		}
	}
}

void FTweenManager::RemoveTweenerWithTarget(TSharedPtr<FFairyTweener> &InTweener, UFairyObject* InTarget)
{
	TweenerTableKey TargetKey = TweenerTableKey(InTarget);
	if ( TweenerTable.Contains(TargetKey) )
	{
		TweenerArray* Array = TweenerTable.Find(TargetKey);
		for (int i = 0; i < Array->Num(); i++)
		{
			TweenerArrayElement element = (*Array)[i];
			if (element.Get() == InTweener.Get())
			{
				Array->RemoveAt(i);
				break;
			}
		}
	}
}

void FTweenManager::RemoveAllTweenerWithTarget(UFairyObject* InTarget)
{
	TweenerTableKey TargetKey = TweenerTableKey(InTarget);
	if ( TweenerTable.Contains(TargetKey) )
	{
		TweenerTable.Remove(TargetKey);
	}
}

bool FTweenManager::KillTween(FTweenerHandle & Handle, bool bCompleted)
{
	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		FFairyTweener* tweener = ActiveTweenerPointerArray[i];
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

bool FTweenManager::KillTweens(UObject* Target, bool bCompleted)
{
	if (Target == nullptr)
	{
		return false;
	}

	bool result = false;
	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		FFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Target.Get() == Target && !tweener->bKilled)
		{
			tweener->Kill(bCompleted);
			result = true;
		}
	}

	return result;
}

FFairyTweener* FTweenManager::GetTween(FTweenerHandle const& Handle)
{
	if (!Handle.IsValid())
	{
		return nullptr;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		FFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Handle == Handle && !tweener->bKilled)
		{
			return tweener;
		}
	}

	return nullptr;
}

FFairyTweener* FTweenManager::GetTween(UObject* Target)
{
	if (Target == nullptr)
	{
		return nullptr;
	}

	int32 cnt = TotalActiveTweenerNum;
	for (int32 i = 0; i < cnt; i++)
	{
		FFairyTweener* tweener = ActiveTweenerPointerArray[i];
		if (tweener != nullptr && tweener->Target.Get() == Target && !tweener->bKilled)
		{
			return tweener;
		}
	}

	return nullptr;
}

void FTweenManager::Tick(float DeltaTime)
{
	for (auto& Pair : TweenerTable)
	{
		TweenerArray& Array = Pair.Value;
		for (size_t i = 0; i < Array.Num(); i++)
		{
			TweenerArrayElement element = Array[i];
			if (!element->IsPaused())
			{
				if (element->IsCompleted() || !element->IsTargetValid())
				{
					CompletedArray.Add(element);
					continue;
				}
				else
				{
					element->Update(DeltaTime);
				}
			}
		}
	}

	for (size_t i = 0; i < CompletedArray.Num(); i++)
	{
		TweenerArrayElement element = CompletedArray[i];
		RemoveTweener(element);
	}
	CompletedArray.Reset();
}