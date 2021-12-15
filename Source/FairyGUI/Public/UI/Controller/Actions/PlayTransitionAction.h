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
    virtual void Enter(UGController* Controller) override;
    virtual void Leave(UGController* Controller) override;

    UTransition* CurrentTransition;
};