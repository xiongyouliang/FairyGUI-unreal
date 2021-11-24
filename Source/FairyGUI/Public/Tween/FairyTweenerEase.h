#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenerInterval.h"
#include "Tween/FairyEaseType.h"
#include "FairyTweenerEase.generated.h"


UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEase : public UFairyTweenerInterval
{
	GENERATED_BODY()
public:
	bool Init(UFairyTweenerInterval* InInnerTweener);

	virtual void StartWithTarget(UFairyObject* InTarget) override;
	virtual void Update(float InTime) override;
	virtual UFairyTweenerInterval* GetInnerTweener();

protected:

	UPROPERTY()
	UFairyTweenerInterval* _innerTweener;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseRate : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	void SetRate(float rate);
	float GetRate();

	bool Init(UFairyTweenerInterval* InInnerTweener, float InRate);

protected:
	float _rate;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseRateIn : public UFairyTweenerEaseRate
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseRateOut : public UFairyTweenerEaseRate
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseRateInOut : public UFairyTweenerEaseRate
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseElastic : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	void SetPeriod(float InPeriod);
	float GetPeriod();

	bool Init(UFairyTweenerInterval* InInnerTweener, float InPeriod);
protected:
	float _period;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseElasticIn : public UFairyTweenerEaseElastic
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseElasticOut : public UFairyTweenerEaseElastic
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseElasticInOut : public UFairyTweenerEaseElastic
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseSineIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseSineOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseSineInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuadIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuadOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuadInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCubicIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCubicOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCubicInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuartIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuartOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuartInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuintIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuintOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseQuintInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseExpoIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseExpoOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseExpoInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCircIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCircOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseCircInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBackIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBackOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBackInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBounceIn : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBounceOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerEaseBounceInOut : public UFairyTweenerEase
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};




