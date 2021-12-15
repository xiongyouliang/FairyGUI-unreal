#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearColor : public FGearBase
{
public:
    FGearColor(UFairyObject* InOwner);
    virtual ~FGearColor();

    virtual void Apply() override;
    virtual void UpdateState() override;

protected:
    virtual void AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer) override;
    virtual void Init() override;

private:
    void OnTweenUpdate(UFairyTweener* Tweener);
    void OnTweenComplete();

    struct FValue
    {
    public:
        FColor Color;
        FColor OutlineColor;

        FValue();
    };

    TMap<FString, FValue> Storage;
    FValue Default;
};