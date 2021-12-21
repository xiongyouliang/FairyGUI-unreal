#pragma once

#include "GearBase.h"

class FAIRYGUI_API FGearDisplay : public FGearBase
{
public:
	FGearDisplay(UFairyObject* InOwner);
	virtual ~FGearDisplay();

	virtual void Apply() override;
	virtual void UpdateState() override;
	virtual bool IsActived() override;

	uint32 AddLock();
	void ReleaseLock(uint32 Token);

	bool IsConnected();
	bool IsOwnerVisible();

	TArray<FName> Pages;

protected:
	virtual void AddStatus(const FName& PageID, FairyGUI::FByteBuffer* Buffer) override;
	virtual void Init() override;

private:
	int32 Visible;
	uint32 DisplayLockToken;
};