#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearFontSize : public FGearBase
{
public:
	FGearFontSize(UFairyObject* InOwner);
	virtual ~FGearFontSize();

	virtual void Apply() override;
	virtual void UpdateState() override;

protected:
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;

private:
	TMap<FName, int32> Storage;
	int32 Default;
};