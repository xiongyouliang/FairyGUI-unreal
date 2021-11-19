#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenerFiniteTime.h"
#include "FairyTweenerInstant.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyTweenerInstant : public UFairyTweenerFiniteTime
{
	GENERATED_BODY()
public:
	virtual void StartWithTarget(UFairyObject* InTarget) override;
	virtual bool IsDone() const override;

	virtual void Step(float InDeltaTime) override;
	virtual void Update(float InTime) override;

protected:
	bool bDone;

};
