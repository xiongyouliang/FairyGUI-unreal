#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "Tween/FairyTweener.h"

#include "TweenManager.generated.h"

class UFairyObject;
class UFairyTweener;
class UFairyTweenerPos;

typedef UFairyTweener* FairyTweenerPointer;

UCLASS(BlueprintType)
class FAIRYGUI_API UTweenManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	
	~UTweenManager();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UTweenManager* Get();

	UFUNCTION(BlueprintCallable)
	UFairyTweenerPos* CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos);

	[[deprecated("Mark to remove in next refactor.")]]
	UFairyTweener* CreateTweener();

	void AddTweener(UFairyTweener* InTweener, UFairyObject* InTarget, bool InPaused);
	UFairyTweener* GetTweenerByTag(int InTag, UFairyObject* InTarget);
	void RemoveTweener(UFairyTweener* InTweener);
	void RemoveTweenerByTag(int InTag, UFairyObject* InTarget);
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
	UFairyTweener* GetTween(FTweenerHandle const& Handle);
	[[deprecated("Mark to remove in next refactor.")]]
	UFairyTweener* GetTween(UObject* Target);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

private:
	UTweenManager(); // hide outside allocate object.
	static UTweenManager* Instance;
	bool bTicking;
	FairyTweenerPointer* ActiveTweenerPointerArray;
	int32 ActiveTweenerPointerCapcity;
	int32 TotalActiveTweenerNum;
	uint32 TweenerInstanceCount;

	TArray<UFairyTweener*> TweenerPool;

	TMap<UFairyObject*, TArray<UFairyTweener*>> TweenerTable;	
	TArray<UFairyTweener*> PreTickRemoveArray;
	TArray<UFairyTweener*> PostTickRemoveArray;

	inline void DoRemoveTweener(UFairyTweener* InTweener);
};

