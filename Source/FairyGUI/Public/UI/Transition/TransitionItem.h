
#pragma once

#include "CoreMinimal.h"
#include "UI/FairyObject.h"
#include "UI/FieldTypes.h"
#include "UI/Transition/TweenConfig.h"

class UTransition;

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
	UTransition* Instance;
	float StopTime;
};

struct FShakeData {
	float Amplitude;
	float Duration;
	FVector2D LastOffset;
	FVector2D Offset;
};

class FTransitionItemBase
{
public:
	ETransitionActionType ActionType;

	float startTime;
	int16 targetID;
	FString label;

	bool bHasTween;

	TSharedPtr<FTweenConfig> TweenConfigPtr; // maybe invalid.

	TSharedPtr<UFairyTweener> TweenerPtr;
	TWeakObjectPtr<UFairyObject> TargetPtr;

	FTransitionItemBase(ETransitionActionType ActionType);
	virtual ~FTransitionItemBase();

	virtual bool IsTargetValid() { return TargetPtr.IsValid(); }

	virtual void Setup(FairyGUI::FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseBaseData(FairyGUI::FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseTweenData(FairyGUI::FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos);
	
	virtual void RunItem() {};
};

FTransitionItemBase* CreateTransitionItemWithType(const ETransitionActionType InActionType);

class FTransitionItemPos : public FTransitionItemBase
{
public:
	FTransitionItemPos();

	float duration;
	KeyFrameDataPos frameData;

	virtual void ParseKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;
	virtual void RunItem() override;
};