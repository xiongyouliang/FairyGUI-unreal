#include "Tween/TweenManager.h"
#include "Tween/GTweener.h"

FTweenManager::FTweenManager()
{
    TotalActiveTweenerNum = 0;
    ActiveTweenerPointerCapcity = 30;
    ActiveTweenerPointers = new FGTweener*[ActiveTweenerPointerCapcity];
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
        FGTweener* tweener = ActiveTweenerPointers[i];
        if (tweener != nullptr)
        {
			delete tweener;
        }
    }
    delete []ActiveTweenerPointers;
}

FGTweener* FTweenManager::CreateTween()
{
    FGTweener* tweener = nullptr;
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
        tweener = new FGTweener();
        tweener->Handle.SetIndex(TweenerInstanceCount);
    }
    tweener->Init();
    ActiveTweenerPointers[TotalActiveTweenerNum++] = tweener;

    if (TotalActiveTweenerNum == ActiveTweenerPointerCapcity)
    {
        int32 newCapcity = ActiveTweenerPointerCapcity + FMath::CeilToInt(ActiveTweenerPointerCapcity * 0.5f);
        FGTweener** newArray = new FGTweener*[newCapcity];
        FMemory::Memcpy(newArray, ActiveTweenerPointers, ActiveTweenerPointerCapcity * sizeof(FGTweener*));
        delete []ActiveTweenerPointers;
        ActiveTweenerPointers = newArray;
        ActiveTweenerPointerCapcity = newCapcity;
    }

    return tweener;
}

bool FTweenManager::KillTween(FTweenerHandle & Handle, bool bCompleted)
{
    int32 cnt = TotalActiveTweenerNum;
    for (int32 i = 0; i < cnt; i++)
    {
        FGTweener* tweener = ActiveTweenerPointers[i];
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
        return false;

    bool flag = false;
    int32 cnt = TotalActiveTweenerNum;
    for (int32 i = 0; i < cnt; i++)
    {
        FGTweener* tweener = ActiveTweenerPointers[i];
        if (tweener != nullptr && tweener->Target.Get() == Target && !tweener->bKilled)
        {
            tweener->Kill(bCompleted);
            flag = true;
        }
    }

    return flag;
}

FGTweener* FTweenManager::GetTween(FTweenerHandle const& Handle)
{
    if (!Handle.IsValid())
    {
		return nullptr;
    }

    int32 cnt = TotalActiveTweenerNum;
    for (int32 i = 0; i < cnt; i++)
    {
        FGTweener* tweener = ActiveTweenerPointers[i];
        if (tweener != nullptr && tweener->Handle == Handle && !tweener->bKilled)
        {
            return tweener;
        }
    }

    return nullptr;
}

FGTweener* FTweenManager::GetTween(UObject* Target)
{
    if (Target == nullptr)
    {
		return nullptr;
    }

    int32 cnt = TotalActiveTweenerNum;
    for (int32 i = 0; i < cnt; i++)
    {
        FGTweener* tweener = ActiveTweenerPointers[i];
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
        FGTweener* tweener = ActiveTweenerPointers[i];
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
            ActiveTweenerPointers[i] = nullptr;

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
                ActiveTweenerPointers[freePosStart] = tweener;
                ActiveTweenerPointers[i] = nullptr;
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
				ActiveTweenerPointers[freePosStart++] = ActiveTweenerPointers[j++];
            }
        }
        TotalActiveTweenerNum = freePosStart;
    }
}