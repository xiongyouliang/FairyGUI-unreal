#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "Tween/FairyTweener.h"

class UFairyObject;

typedef FFairyTweener* FairyTweenerPointer;

class FAIRYGUI_API FTweenManager : public FTickableGameObject
{
public:
    FTweenManager();
    ~FTweenManager();

    FFairyTweener* CreateTweener();

    bool IsTweening(FTweenerHandle const& Handle) { return GetTween(Handle) != nullptr; }
    bool IsTweening(UObject* Target) { return GetTween(Target) != nullptr; }

    bool KillTween(FTweenerHandle & Handle, bool bCompleted);
    bool KillTweens(UObject* Target, bool bCompleted);

    FFairyTweener* GetTween(FTweenerHandle const& Handle);
    FFairyTweener* GetTween(UObject* Target);

    void Tick(float DeltaTime);
    TStatId GetStatId() const 
    {
        return TStatId();
    }

private:
    FairyTweenerPointer* ActiveTweenerPointerArray;
    int32 ActiveTweenerPointerCapcity;
    int32 TotalActiveTweenerNum;
    uint32 TweenerInstanceCount;

    TArray<FairyTweenerPointer> TweenerPool;
};

