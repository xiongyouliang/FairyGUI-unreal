#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearAnimation : public FGearBase
{
public:
	FGearAnimation(UFairyObject* InOwner);
	virtual ~FGearAnimation();

	virtual void Apply() override;
	virtual void UpdateState() override;

protected:
	virtual void AddStatus(const FString& PageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;

private:
	struct FValue
	{
		bool bPlaying;
		int32 Frame;

		FValue();
	};
	TMap<FString, FValue> Storage;
	FValue Default;
};