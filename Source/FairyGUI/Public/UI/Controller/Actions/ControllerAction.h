#pragma once

#include "CoreMinimal.h"
#include "Utils/ByteBuffer.h"

class UFairyController;

class FControllerAction
{
public:
    static FControllerAction* CreateAction(int32 ActionType);

    FControllerAction();
    virtual ~FControllerAction();

    void Run(UFairyController* Controller, const FString& PreviousPage, const FString& CurrentPage);
    virtual void Setup(FairyGUI::FByteBuffer* Buffer);

    TArray<FString> FromPage;
    TArray<FString> ToPage;

protected:
    virtual void Enter(UFairyController* Controller) = 0;
    virtual void Leave(UFairyController* Controller) = 0;
};