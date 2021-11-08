#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyTweenHelper.h"

class UFairyObject;
class UGController;
class FByteBuffer;

class FAIRYGUI_API FGearTweenConfig
{
public:
    FGearTweenConfig();

    bool bTween;
    EEaseType EaseType;
    float Duration;
    float Delay;

    uint32 DisplayLockToken;
    FTweenerHandle Handle;
};

class FGearBase
{
public:
    enum EGearType
    {
        Display = 0,
        XY,
        Size,
        Look,
        Color,
        Animation,
        Text,
        Icon,
        Display2,
        FontSize
    };

    FGearBase(UFairyObject* InOwner);
    FGearBase(UFairyObject* InOwner, EGearType InType);
    virtual ~FGearBase();

    EGearType GetType() const { return Type; }

    UGController* GetController() const;
    void SetController(UGController* InController);
    void HandleControllerChanged(UGController* InController);

    FGearTweenConfig& GetTweenConfig();

    virtual void UpdateFromRelations(const FVector2D& Delta);
    virtual void Apply();
    virtual void UpdateState();

    virtual void Setup(FByteBuffer* Buffer);

    static FGearBase* Create(UFairyObject* InOwner, EGearType InType);
    static bool bDisableAllTweenEffect;

protected:
    virtual void AddStatus(const FString& PageID, FByteBuffer* Buffer);
    virtual void Init();

    EGearType Type;
    TWeakObjectPtr<UFairyObject> Owner;
    TWeakObjectPtr<UGController> Controller;
    TOptional<FGearTweenConfig> TweenConfig;

    FDelegateHandle ControllerHandle;
};