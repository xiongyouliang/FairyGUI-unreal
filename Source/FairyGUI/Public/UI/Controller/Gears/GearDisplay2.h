#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearDisplay2 : public FGearBase
{
public:
	FGearDisplay2(UFairyObject* InOwner);
	virtual ~FGearDisplay2();

	virtual void Apply() override;
	virtual void UpdateState() override;
	virtual bool IsActived() override;
	bool Evaluate(bool bConnected);

	TArray<FString> Pages;

	enum EConditionType
	{
		And = 0,
		Or = 1
	};
	EConditionType Condition;

protected:
	virtual void AddStatus(const FString& pageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;
};