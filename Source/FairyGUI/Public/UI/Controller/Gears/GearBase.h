#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyEaseType.h"
#include "Tween/FairyTweener.h"
#include "Utils/ByteBuffer.h"
#include "UI/Controller/Gears/FairyGearType.h"

class UFairyObject;
class UFairyController;
class FByteBuffer;

class FAIRYGUI_API FGearTweenConfig
{
public:
	FGearTweenConfig();

	bool bTween;
	EFairyEaseType EaseType;
	float Duration;
	float Delay;

	uint32 DisplayLockToken;
};

class FGearBase
{
public:

	FGearBase(UFairyObject* InTarget);
	FGearBase(UFairyObject* InTarget, EFairyGearType InType);
	virtual ~FGearBase();

	EFairyGearType GetType() const { return Type; }

	UFairyController* GetController() const;
	void SetController(UFairyController* InController);
	void HandleControllerChanged(UFairyController* InController);

	UFairyObject* GetTarget();

	FGearTweenConfig& GetTweenConfig();

	virtual void UpdateFromRelations(const FVector2D& Delta);
	virtual void Apply();
	virtual void UpdateState();

	virtual bool IsActived();

	virtual void Setup(FairyGUI::FByteBuffer* Buffer);

	/**
	* This is a Factory Method. All Gear object Come from here.
	* If you need a gear object pool for reuseable, just change the Factory Method implementations.
	*/
	static FGearBase* Create(UFairyObject* InTarget, EFairyGearType InType);

	static bool bDisableAllTweenEffect;

protected:
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer);
	virtual void Init();

	EFairyGearType Type;
	TWeakObjectPtr<UFairyObject> TargetObject;
	TWeakObjectPtr<UFairyController> Controller;
	TOptional<FGearTweenConfig> TweenConfig;

	FDelegateHandle ControllerHandle;
};
