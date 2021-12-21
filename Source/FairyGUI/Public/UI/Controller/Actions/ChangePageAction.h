#pragma once

#include "ControllerAction.h"

class FChangePageAction : public FControllerAction
{
public:
    virtual void Setup(FairyGUI::FByteBuffer* Buffer) override;

    FName ObjectID;
    FName ControllerName;
    FName TargetPage;

protected:
    virtual void Enter(UFairyController* Controller) override;
    virtual void Leave(UFairyController* Controller) override;
};