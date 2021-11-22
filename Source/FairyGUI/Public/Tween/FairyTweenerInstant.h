#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenerFiniteTime.h"
#include "FairyTweenerInstant.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerInstant : public UFairyTweenerFiniteTime
{
	GENERATED_BODY()
public:
	virtual bool IsInstant() override;
	virtual void StartWithTarget(UFairyObject* InTarget) override;
	virtual bool IsDone() const override;

	virtual void Step(float InDeltaTime) override;
	virtual void Update(float InTime) override;

protected:
	bool bDone;

};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerShow : public UFairyTweenerInstant
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerHide : public UFairyTweenerInstant
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerPivot : public UFairyTweenerInstant
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
	void Init(FVector2D InNewPivot);

protected:
	FVector2D newPivot;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerCallFunc : public UFairyTweenerInstant
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
	bool Init(const FTweenDelegate& InDelegate);
protected:
	FTweenDelegate _delegate;
};

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerDynamicCallFunc : public UFairyTweenerInstant
{
	GENERATED_BODY()
public:
	virtual void Update(float InTime) override;
	bool Init(const FTweenDynamicDelegate& InDelegate);
protected:
	FTweenDynamicDelegate _delegate;
};
