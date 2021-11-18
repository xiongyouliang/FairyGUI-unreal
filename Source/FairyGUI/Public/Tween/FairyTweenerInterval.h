#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenerFiniteTime.h"

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
	virtual void StartWithTarget(UFairyObject* InTarget) override;

	void Init(float InDuration);

protected:
	float elapsedTime;
	bool bFirstTick;
	bool bDone;
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