
#pragma once

#include "CoreMinimal.h"
#include "Utils/ByteBuffer.h"
#include "UI/FairyObject.h"
#include "UI/FieldTypes.h"
#include "UI/Transition/FairyTransitionTweenConfig.h"

class UFairyTransition;
class UFairyTweenerFiniteTime;

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
	UFairyTransition* Instance;
	float StopTime;
};

struct FShakeData {
	float Amplitude;
	float Duration;
	FVector2D LastOffset;
	FVector2D Offset;
};

class FFairyTransitionItemBase
{
protected:
	bool bHasTween;
	int16 TargetIndex;
	EFairyTransitionItemType ItemType;
	float StartTime;
	FName Label;

	TWeakObjectPtr<UFairyTransition> OwnerPtr;
	TWeakObjectPtr<UFairyObject> TargetPtr;

public:
	FFairyTransitionItemBase(EFairyTransitionItemType InItemType);
	virtual ~FFairyTransitionItemBase();
	
	void SetOwner(UFairyTransition* InOwner);
	UFairyTransition* GetOwner();

	EFairyTransitionItemType GetTimelineType() { return ItemType; }

	void SetTarget(UFairyObject* InTargetObject);
	UFairyObject* GetTarget();

	float GetStartTime() { return StartTime; }
	
	virtual float GetDuration() { return 0.0f; }

	virtual bool IsTargetValid();
	virtual bool IsHasTween();

	

	virtual void Setup(FairyGUI::FByteBuffer* InBuffer, int32 curPos);

	virtual void ParseBaseData(FairyGUI::FByteBuffer* InBuffer, int32 curPos);
	virtual void ParseTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) {};
	virtual void ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) {};
	
	virtual void RunItem() {};
	virtual void ConstructTweenerList(TArray<UFairyTweenerFiniteTime*>& OutTweenerList, FFairyTransitionItemBase* InPreviousItem) {};
};

class FFairyTransitionTweenableItem : public FFairyTransitionItemBase
{
protected:
	TSharedPtr<FFariyTransitionTweenConfig> TweenConfigPtr; // maybe invalid.

	TSharedPtr<UFairyTweener> TweenerPtr;

public:
	FFairyTransitionTweenableItem(EFairyTransitionItemType InItemType);

	virtual float GetDuration() { return TweenConfigPtr->Duration; };

	virtual void ParseTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;
	virtual void ParseNoTweenKeyFrameData(FairyGUI::FByteBuffer* InBuffer, int32 curPos) override;

	virtual void ParseTweenStartData(FairyGUI::FByteBuffer* InBuffer) {};
	virtual void ParseTweenEndData(FairyGUI::FByteBuffer* InBuffer) {};

	/**
	* currently, only position animation can use path. 
	* so this is a empty function for other animation.
	* position transition item need to override this method to parse path data when used in editor.
	*/ 
	virtual void ParsePathData(FairyGUI::FByteBuffer* InBuffer) {}; 
};
