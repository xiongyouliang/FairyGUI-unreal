#pragma once

#include "CoreMinimal.h"
#include "Tween/FairyEaseType.h"
#include "Tween/FairyTweener.h"
#include "Utils/ByteBuffer.h"

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

    UFairyController* GetController() const;
    void SetController(UFairyController* InController);
    void HandleControllerChanged(UFairyController* InController);

    FGearTweenConfig& GetTweenConfig();

    virtual void UpdateFromRelations(const FVector2D& Delta);
    virtual void Apply();
    virtual void UpdateState();

    virtual void Setup(FairyGUI::FByteBuffer* Buffer);

    static FGearBase* Create(UFairyObject* InOwner, EGearType InType);
    static bool bDisableAllTweenEffect;

protected:
    virtual void AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer);
    virtual void Init();

    EGearType Type;
    TWeakObjectPtr<UFairyObject> Owner;
    TWeakObjectPtr<UFairyController> Controller;
    TOptional<FGearTweenConfig> TweenConfig;

    FDelegateHandle ControllerHandle;
};