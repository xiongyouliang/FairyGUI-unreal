#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tickable.h"
#include "UI/FairyObject.h"
#include "Tween/FairyTweener.h"
#include "Tween/FairyTweenerInterval.h"
#include "Tween/FairyTweenerInstant.h"
#include "Tween/FairyTweenerEase.h"

#include "FairyTweenMgr.generated.h"

typedef UFairyTweener* FairyTweenerPointer;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerArrayWrapper : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TArray<UFairyTweener*> TweenerList;

public:
	void AddTweener(UFairyTweener* InTweener) { TweenerList.Push(InTweener); }
	void RemoveTweener(UFairyTweener* InTweener);
	int32 Num() { return TweenerList.Num(); };

	UFairyTweener* operator[](size_t index) { return TweenerList[index]; }
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenMgr : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	
	~UFairyTweenMgr();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerPos* CreateTweenerPos(float InDuration, FVector2D InStartPos, FVector2D InDstPos);
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerSize* CreateTweenerSize(float InDuration, FVector2D InStartSize, FVector2D InDstSize);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerOpacity* CreateTweenerOpacity(float InDuration, float InStartOpacity, float InDstOpacity);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerRotate* CreateTweenerRotate(float InDuration, float InStartroRotate, float InDstRotate);
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerScale* CreateTweenerScale(float InDuration, FVector2D InStartScale, FVector2D InDstScale);
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerSkew* CreateTweenerSkew(float InDuration, FVector2D InStartSkew, FVector2D InDstSkew);
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerColor* CreateTweenerColor(float InDuration, FColor InStartColor, FColor InDstColor);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerSequence* CreateTweenerSequence(const TArray<UFairyTweenerFiniteTime*>& InTweenerList);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerDelay* CreateTweenerDelay(float InDelayTime);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerShow* CreateTweenerShow();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerHide* CreateTweenerHide();
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerPivot* CreateTweenerPivot(FVector2D InDstPivot);

	// non-dynamic delegate can't pass in blueprint, so this is not a UFUNCTION
	UFairyTweenerCallFunc* CreateTweenerCallFunc(const FTweenDelegate& InDelegate);	
	
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerDynamicCallFunc* CreateTweenerDynamicCallFunc(const FTweenDynamicDelegate& InDelegate);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyTweenerEase* CreateTweenerEase(UFairyTweenerInterval* InInnerTweener, EFairyEaseType InEaseType, float InArgs);


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
	friend class UFairyApplication;
	UFairyTweenMgr(); // hide outside allocate object.
	static UFairyTweenMgr* Instance;
	bool bTicking;

	UPROPERTY()
	TMap<UFairyObject*, UFairyTweenerArrayWrapper*> TweenerTable;
	
	UPROPERTY()
	TArray<UFairyTweener*> PrePendingRemoveArray;

	inline void DoRemoveTweener(UFairyTweener* InTweener);
};

