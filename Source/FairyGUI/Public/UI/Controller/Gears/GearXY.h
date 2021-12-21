#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearXY : public FGearBase
{
public:
	FGearXY(UFairyObject* InOwner);
	virtual ~FGearXY();

	virtual void Apply() override;
	virtual void UpdateState() override;
	virtual void UpdateFromRelations(const FVector2D& Delta) override;

	bool bPositionsInPercent;
	void AddExtStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer);

protected:
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;

private:
	void OnTweenComplete();

	TMap<FName, FVector4> Storage;
	FVector4 Default;
};