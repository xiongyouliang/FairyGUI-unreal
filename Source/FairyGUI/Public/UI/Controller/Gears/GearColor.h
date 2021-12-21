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
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer) override;
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

	TMap<FName, FValue> Storage;
	FValue Default;
};