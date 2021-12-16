#include "UI/Controller/Actions/ControllerAction.h"
#include "UI/Controller/Actions/ChangePageAction.h"
#include "UI/Controller/Actions/PlayTransitionAction.h"
#include "Utils/ByteBuffer.h"

FControllerAction * FControllerAction::CreateAction(int32 ActionType)
{
	switch (ActionType)
	{
	case 0:
		return new FPlayTransitionAction();

	case 1:
		return new FChangePageAction();
	}
	return nullptr;
}

FControllerAction::FControllerAction()
{
}

FControllerAction::~FControllerAction()
{
}

void FControllerAction::Run(UFairyController* Controller, const FString& PreviousPage, const FString& CurrentPage)
{
	if ((FromPage.Num() == 0 || FromPage.Contains(PreviousPage))
		&& (ToPage.Num() == 0 || ToPage.Contains(CurrentPage)))
		Enter(Controller);
	else
		Leave(Controller);
}

void FControllerAction::Setup(FairyGUI::FByteBuffer* Buffer)
{
	int32 cnt;

	cnt = Buffer->ReadShort();
	FromPage.SetNum(cnt);
	for (int32 i = 0; i < cnt; i++)
	{
		FromPage[i] = Buffer->ReadStringFromCache();
	}

	cnt = Buffer->ReadShort();
	ToPage.SetNum(cnt);
	for (int32 i = 0; i < cnt; i++)
	{
		ToPage[i] = Buffer->ReadStringFromCache();
	}
}
