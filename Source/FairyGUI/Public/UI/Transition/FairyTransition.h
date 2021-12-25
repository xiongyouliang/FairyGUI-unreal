#pragma once

#include "CoreMinimal.h"
#include "Event/EventContext.h"
#include "UI/Transition/FairyTransitionItemBase.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"
#include "FairyTransition.generated.h"

class UFairyObject;
class UFairyComponent;
class UFairyController;
class FByteBuffer;
class UFairyTweener;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTransition : public UObject
{
    GENERATED_BODY()
public:
    UFairyTransition();
    virtual ~UFairyTransition();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI | Transition")
    FName GetName();

    UFairyComponent* GetTargetComponent() { return TargetComponent; }
    void SetTargetComponent(UFairyComponent* InTargetComponent) { TargetComponent = InTargetComponent; }

    bool IsPlaying() const { return bPlaying; }
    void Play(const FSimpleDelegate& InCompleteCallback = FSimpleDelegate())
    {
        Play(1, 0, 0, -1, false, InCompleteCallback);
    }

    void Play(int32 InTimes, float InDelay, float InStartTime = 0, float InEndTime = -1, const FSimpleDelegate& InCompleteCallback = FSimpleDelegate())
    {
        Play(InTimes, InDelay, InStartTime, InEndTime, false, InCompleteCallback);
    }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "InCompleteCallback"))
    void Play(const FSimpleDynDelegate& InCompleteCallback, int32 InTimes = 1, float InDelay = 0, float InStartTime = 0, float InEndTime = -1)
    {
        Play(InTimes, InDelay, InStartTime, InEndTime, false, 
            InCompleteCallback.IsBound() ?
            FSimpleDelegate::CreateUFunction(const_cast<UObject*>(InCompleteCallback.GetUObject()), InCompleteCallback.GetFunctionName())
            : FSimpleDelegate());
    }

    void PlayReverse(const FSimpleDelegate& InCompleteCallback = FSimpleDelegate())
    {
        Play(1, 0, 0, -1, true, InCompleteCallback);
    }

    void PlayReverse(int32 InTimes, float InDelay, const FSimpleDelegate& InCompleteCallback = FSimpleDelegate())
    {
        Play(InTimes, InDelay, 0, -1, true, InCompleteCallback);
    }
    
    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "InCompleteCallback"))
    void PlayReverse(const FSimpleDynDelegate& InCompleteCallback, int32 InTimes = 1, float InDelay = 0)
    {
        Play(InTimes, InDelay, 0, -1, true, 
            InCompleteCallback.IsBound() ?
            FSimpleDelegate::CreateUFunction(const_cast<UObject*>(InCompleteCallback.GetUObject()), InCompleteCallback.GetFunctionName())
            : FSimpleDelegate());
    }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ChangePlayTimes(int32 InTimes);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void Stop(bool bSetToComplete = false, bool bProcessCallback = false);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetAutoPlay(bool bInAutoPlay, int32 InTimes, float InDelay);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPaused(bool bInPaused);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetValue(const FString& InLabel, const TArray<FNVariant>& InValues);

    void SetHook(const FString& InLabel, FSimpleDelegate Callback);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetHook(const FString& InLabel, FSimpleDynDelegate Callback)
    {
        SetHook(InLabel, Callback.IsBound() ?
            FSimpleDelegate::CreateUFunction(const_cast<UObject*>(Callback.GetUObject()), Callback.GetFunctionName())
            : FSimpleDelegate());
    }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ClearHooks();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetTarget(const FString& InLabel, UFairyObject* InTarget);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetDuration(const FString& InLabel, float InDuration);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetLabelTime(const FString& InLabel) const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetTimeScale() const { return TimeScale; }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetTimeScale(float InTimeScale);

    void UpdateFromRelations(const FString& TargetID, const FVector2D& Delta);
    void OnOwnerAddedToStage();
    void OnOwnerRemovedFromStage();

    void Setup(FairyGUI::FByteBuffer* Buffer);

   

private:
    void Play(int32 InTimes, float InDelay, float InStartTime, float InEndTime, bool bInReverse, FSimpleDelegate InCompleteCallback);
    void StopItem(FFairyTransitionItemBase* Item, bool bSetToComplete);
    void OnDelayedPlay();
    void InternalPlay();
    void PlayItem(FFairyTransitionItemBase* Item);
    void SkipAnimations();
    void OnDelayedPlayItem(UFairyTweener* Tweener);
    void OnTweenStart(UFairyTweener* Tweener);
    void OnTweenUpdate(UFairyTweener* Tweener);
    void OnTweenComplete(UFairyTweener* Tweener);
    void OnPlayTransCompleted(FFairyTransitionItemBase* item);
    void CallHook(FFairyTransitionItemBase* Item, bool bTweenEnd);
    void CheckAllComplete();
    void ApplyValue(FFairyTransitionItemBase* Item);


    FName Name;

    UPROPERTY()
    UFairyComponent* TargetComponent;

    TArray<TSharedPtr<FFairyTransitionItemBase>> Items;

    bool bPlaying;
    bool bPaused;
    bool bAutoPlay;
    bool bReversed;

	int32 TotalTimes;
	int32 TotalTasks;
    int32 Options;
    int32 AutoPlayTimes;
    
    float TotalDuration;
    float AutoPlayDelay;
    float TimeScale;
    float StartTime;
    float EndTime;

	FVector2D OwnerBasePos;
	FSimpleDelegate CompleteCallback;
};