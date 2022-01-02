#pragma once

#include "CoreMinimal.h"
#include "UI/Tween/FairyTweenerFiniteTime.h"
#include "FairyTweenerInterval.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerInterval : public UFairyTweenerFiniteTime
{
	GENERATED_BODY()
public:
	float GetElapsedTime();

	virtual void SetAmplitudeRate(float InAmplitudeRate);
	virtual float GetAmplitudeRate();

	virtual bool IsDone() const override;
	virtual void Step(float InDeltaTime) override;
	virtual void SetTarget(UFairyObject* InTarget) override;

	virtual void Reset() override;

	void Init(float InDuration);

protected:
	float elapsedTime;
	bool bFirstTick;
	bool bDone;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerDelay : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration);

	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerSequence : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(const TArray<UFairyTweenerFiniteTime*>& InTweenerList);

	virtual void Step(float InDeltaTime) override;
	virtual void SetTarget(UFairyObject* InTarget) override;
	virtual void Reset() override;
protected:
	UPROPERTY()
	TArray<UFairyTweenerFiniteTime*> tweenerList;
	size_t curIndex = 0;
	bool bFirstIntervalTweener = true;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerRepeat : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(UFairyTweenerFiniteTime* InTweener, uint32 repeatTimes);

	virtual void Step(float InDeltaTime) override;
	virtual void SetTarget(UFairyObject* InTarget) override;

private:
	uint32 m_passedTimes;
	uint32 m_repeatTimes;

	UPROPERTY()
	UFairyTweenerFiniteTime* _innerTweener;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerPos : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, FVector2D InStartPos, FVector2D InDstPos);

	virtual void Update(float InTime) override;

protected:
	FVector2D startPos;
	FVector2D dstPos;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerSize : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, FVector2D InStartSize, FVector2D InDstSize);

	virtual void Update(float InTime) override;

protected:
	FVector2D startSize;
	FVector2D dstSize;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerOpacity : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, float InStartOpacity, float InDstOpacity);

	virtual void Update(float InTime) override;

protected:
	float startOpacity;
	float dstOpacity;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerRotate : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, float InStartRotate, float InDstRotate);
	virtual void Update(float InTime) override;

protected:
	float startRotate;
	float dstRotate;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerScale : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, FVector2D InStartScale, FVector2D InDstScale);
	virtual void Update(float InTime) override;

protected:
	FVector2D startScale;
	FVector2D dstScale;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerSkew : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, FVector2D InStartSkew, FVector2D InDstSkey);
	virtual void Update(float InTime) override;

protected:
	FVector2D startSkew;
	FVector2D dstSkew;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerColor : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, FColor InStartColor, FColor InDstColor);
	virtual void Update(float InTime) override;

protected:
	FColor startColor;
	FColor dstColor;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerShake : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(float InDuration, float Amplitude);

	virtual void SetTarget(UFairyObject* InTarget);
	virtual void Step(float InDeltaTime) override;

protected:
	float Amplitude;
	int32 Direction; // 0, 1, 2, 3 means left, top, right and bottom, and looped in clockwise direction
	FVector2D originPos;
};



