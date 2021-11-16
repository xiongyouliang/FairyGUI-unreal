#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "Tween/FairyTweener.h"

class UFairyObject;

typedef FFairyTweener* FairyTweenerPointer;
typedef TWeakObjectPtr<UFairyObject> TweenerTableKey;
typedef TSharedPtr<FFairyTweener> TweenerArrayElement;
typedef TArray<TweenerArrayElement> TweenerArray;

class FAIRYGUI_API FTweenManager : public FTickableGameObject
{
public:
	FTweenManager();
	~FTweenManager();

	[[deprecated("Mark to remove in next refactor.")]]
	FFairyTweener* CreateTweener();

	void AddTweener(TSharedPtr<FFairyTweener> &InTweener, UFairyObject* InTarget, bool InPaused);
	void RemoveTweener(TSharedPtr<FFairyTweener> &InTweener);
	void RemoveTweenerWithTarget(TSharedPtr<FFairyTweener> &InTweener, UFairyObject* InTarget);
	void RemoveAllTweenerWithTarget(UFairyObject* InTarget);

	[[deprecated("Mark to remove in next refactor.")]]
	bool IsTweening(FTweenerHandle const& Handle) { return GetTween(Handle) != nullptr; }
	[[deprecated("Mark to remove in next refactor.")]]
	bool IsTweening(UObject* Target) { return GetTween(Target) != nullptr; }

	[[deprecated("Mark to remove in next refactor.")]]
	bool KillTween(FTweenerHandle & Handle, bool bCompleted);
	[[deprecated("Mark to remove in next refactor.")]]
	bool KillTweens(UObject* Target, bool bCompleted);

	[[deprecated("Mark to remove in next refactor.")]]
	FFairyTweener* GetTween(FTweenerHandle const& Handle);
	[[deprecated("Mark to remove in next refactor.")]]
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

	TArray<FFairyTweener*> TweenerPool;
	TMap<TweenerTableKey, TweenerArray> TweenerTable;
	TweenerArray CompletedArray;
};

