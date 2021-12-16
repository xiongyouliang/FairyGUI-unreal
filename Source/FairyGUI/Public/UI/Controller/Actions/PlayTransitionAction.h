#pragma once

#include "ControllerAction.h"
#include "Utils/ByteBuffer.h"

class UTransition;

class FPlayTransitionAction : public FControllerAction
{
public:
    FPlayTransitionAction();
    virtual void Setup(FairyGUI::FByteBuffer* Buffer) override;

    FString TransitionName;
    int32 PlayTimes;
    float Delay;
    bool bStopOnExit;

protected:
    virtual void Enter(UFairyController* Controller) override;
    virtual void Leave(UFairyController* Controller) override;

    UTransition* CurrentTransition;
};