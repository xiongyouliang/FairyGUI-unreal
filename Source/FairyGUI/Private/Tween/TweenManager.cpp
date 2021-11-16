#include "Tween/TweenManager.h"
#include "Tween/GTweener.h"

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
    int32 cnt = TotalActiveTweenerNum;
    int32 freePosStart = -1;
    for (int32 i = 0; i < cnt; i++)
    {
        FFairyTweener* tweener = ActiveTweenerPointerArray[i];
        if (tweener == nullptr)
        {
            if (freePosStart == -1)
            {
				freePosStart = i;
            }
        }
        else if (tweener->bKilled)
        {
            tweener->Reset();
            TweenerPool.Add(tweener);
            ActiveTweenerPointerArray[i] = nullptr;

            if (freePosStart == -1)
            {
				freePosStart = i;
            }
        }
        else
        {
            if (tweener->Target.IsStale())
            {
				tweener->bKilled = true;
            }
            else if (!tweener->bPaused)
            {
				tweener->Update(DeltaTime);
            }

            if (freePosStart != -1)
            {
                ActiveTweenerPointerArray[freePosStart] = tweener;
                ActiveTweenerPointerArray[i] = nullptr;
                freePosStart++;
            }
        }
    }

    if (freePosStart >= 0)
    {
        if (TotalActiveTweenerNum != cnt) //new tweens added
        {
            int32 j = cnt;
            cnt = TotalActiveTweenerNum - cnt;
            for (int32 i = 0; i < cnt; i++)
            {
				ActiveTweenerPointerArray[freePosStart++] = ActiveTweenerPointerArray[j++];
            }
        }
        TotalActiveTweenerNum = freePosStart;
    }
}