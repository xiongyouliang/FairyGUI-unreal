#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "GTweener.h"

class UFairyObject;

class FAIRYGUI_API FTweenManager : public FTickableGameObject
{
public:
    FTweenManager();
    ~FTweenManager();

    FGTweener* CreateTween();

    bool IsTweening(FTweenerHandle const& Handle) { return GetTween(Handle) != nullptr; }
    bool IsTweening(UObject* Target) { return GetTween(Target) != nullptr; }

    bool KillTween(FTweenerHandle & Handle, bool bCompleted);
    bool KillTweens(UObject* Target, bool bCompleted);

    FGTweener* GetTween(FTweenerHandle const& Handle);
    FGTweener* GetTween(UObject* Target);

    void Tick(float DeltaTime);
    TStatId GetStatId() const 
    {
        return TStatId();
    }

private:
    FGTweener** ActiveTweenerPointers;
    TArray<FGTweener*> TweenerPool;
    int32 ActiveTweenerPointerCapcity;
    int32 TotalActiveTweenerNum;
    uint32 TweenerInstanceCount;
};