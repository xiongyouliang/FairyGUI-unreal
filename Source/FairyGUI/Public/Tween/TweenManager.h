#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "Tween/FairyTweener.h"

#include "TweenManager.generated.h"

class UFairyObject;
class UFairyTweener;
class UFairyTweenerFiniteTime;
class UFairyTweenerPos;
class UFairyTweenerSequence;

typedef UFairyTweener* FairyTweenerPointer;

UCLASS(BlueprintType)
class FAIRYGUI_API UTweenManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	
	~UTweenManager();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UTweenManager* Get();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerPos* CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerSequence* CreateTweenerSequence(const TArray<UFairyTweenerFiniteTime*>& InTweenerList);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerDelay* CreateTweenerDelay(float InDelayTime);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void AddTweener(UFairyTweener* InTweener, UFairyObject* InTarget, bool InPaused);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweener* GetTweenerByTag(int InTag, UFairyObject* InTarget);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveTweener(UFairyTweener* InTweener);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveTweenerByTag(int InTag, UFairyObject* InTarget);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveAllTweenerWithTarget(UFairyObject* InTarget);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

private:
	UTweenManager(); // hide outside allocate object.
	static UTweenManager* Instance;
	bool bTicking;

	TMap<UFairyObject*, TArray<UFairyTweener*>> TweenerTable;	
	TArray<UFairyTweener*> PreTickRemoveArray;
	TArray<UFairyTweener*> PostTickRemoveArray;

	inline void DoRemoveTweener(UFairyTweener* InTweener);
};

