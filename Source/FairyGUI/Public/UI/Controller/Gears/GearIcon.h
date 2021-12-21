#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearIcon : public FGearBase
{
public:
	FGearIcon(UFairyObject* InOwner);
	virtual ~FGearIcon();

	virtual void Apply() override;
	virtual void UpdateState() override;

protected:
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;

private:
	TMap<FName, FString> Storage;
	FString Default;
};