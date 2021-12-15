#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearText : public FGearBase
{
public:
    FGearText(UFairyObject* InOwner);
    virtual ~FGearText();

    virtual void Apply() override;
    virtual void UpdateState() override;

protected:
    virtual void AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer) override;
    virtual void Init() override;

private:
    TMap<FString, FString> Storage;
    FString Default;
};