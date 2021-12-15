#pragma once

#include "ControllerAction.h"

class FChangePageAction : public FControllerAction
{
public:
    virtual void Setup(FairyGUI::FByteBuffer* Buffer) override;

    FString ObjectID;
    FString ControllerName;
    FString TargetPage;

protected:
    virtual void Enter(UGController* Controller) override;
    virtual void Leave(UGController* Controller) override;
};