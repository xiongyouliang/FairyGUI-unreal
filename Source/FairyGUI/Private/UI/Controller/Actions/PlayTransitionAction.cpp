#include "UI/Controller/Actions/PlayTransitionAction.h"
#include "UI/Controller/FairyController.h"
#include "UI/Transition/Transition.h"
#include "Utils/ByteBuffer.h"

FPlayTransitionAction::FPlayTransitionAction() :
    PlayTimes(1),
    Delay(0),
    bStopOnExit(false),
    CurrentTransition(nullptr)
{
}

void FPlayTransitionAction::Setup(FairyGUI::FByteBuffer* Buffer)
{
    FControllerAction::Setup(Buffer);

    TransitionName = Buffer->ReadStringFromCache();
    PlayTimes = Buffer->ReadInt();
    Delay = Buffer->ReadFloat();
    bStopOnExit = Buffer->ReadBool();
}

void FPlayTransitionAction::Enter(UFairyController* Controller)
{
    UTransition* trans = Cast<UFairyComponent>(Controller->GetOuter())->GetTransition(TransitionName);
    if (trans != nullptr)
    {
        if (CurrentTransition != nullptr && CurrentTransition->IsPlaying())
        {
            trans->ChangePlayTimes(PlayTimes);
        }
        else
        {
            trans->Play(PlayTimes, Delay);
        }
        CurrentTransition = trans;
    }
}

void FPlayTransitionAction::Leave(UFairyController* Controller)
{
    if (bStopOnExit && CurrentTransition != nullptr)
    {
        CurrentTransition->Stop();
        CurrentTransition = nullptr;
    }
}