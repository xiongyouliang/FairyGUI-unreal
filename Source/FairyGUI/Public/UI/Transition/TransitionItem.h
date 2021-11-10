
#pragma once

#include "CoreMinimal.h"
#include "UI/FairyObject.h"
#include "UI/FieldTypes.h"
#include "UI/Transition/Transition.h"
#include "UI/Transition/TweenConfig.h"
#include "UI/Transition/TransitionItemData.h"

struct FAniData {
	int32 Frame;
	bool bPlaying;
	bool bFlag;
};

struct FSoundData {
	FString URL;
	float Volume;
};

struct FInnerTransData {
	FString Name;
	int32 PlayTimes;
	UTransition *Instance;
	float StopTime;
};

struct FShakeData {
	float Amplitude;
	float Duration;
	FVector2D LastOffset;
	FVector2D Offset;
};

struct FTransitionItem
{
	float Time;
	FString TargetID;
	ETransitionActionType Type;
	TOptional<FTweenConfig> TweenConfig;
	FString Label;
	FSimpleDelegate Hook;

	TOptional<FTransitionItemData> Data;
	TOptional<bool> VisibleData;
	TOptional<FAniData> AniData;
	TOptional<FSoundData> SoundData;
	TOptional<FInnerTransData> TransData;
	TOptional<FShakeData> ShakeData;
	TOptional<FString> TextData;

	//running properties
	FGTweener* Tweener;
	UFairyObject* Target;
	uint32 DisplayLockToken;

	FTransitionItem(ETransitionActionType aType);
	~FTransitionItem();
};

class FTransitionItemBase
{
public:
	ETransitionActionType ActionType;

	float startTime;
	int32 targetID;
	FString label;

	bool bHasTween;

	TSharedPtr<FTweenConfig> TweenConfigPtr; // maybe invalid.

	TSharedPtr<FGTweener> TweenerPtr;
	TWeakObjectPtr<UFairyObject> TargetPtr;

	FTransitionItemBase(ETransitionActionType ActionType);
	virtual ~FTransitionItemBase();

	virtual bool IsTargetValid() { return TargetPtr.IsValid(); }

	virtual void Setup(FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseBaseData(FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseTweenData(FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseKeyFrameData(FByteBuffer* InBuffer, int32 curPos);
	
	virtual void RunItem() {};
};

FTransitionItemBase* CreateTransitionItemWithType(const ETransitionActionType InActionType);

class FTransitionItemPos : public FTransitionItemBase
{
public:
	FTransitionItemPos();

	float duration;
	FVector2D StartPos;
	FVector2D EndPos;

	virtual void ParseKeyFrameData(FByteBuffer* InBuffer, int32 curPos) override;
	virtual void RunItem() override;
};