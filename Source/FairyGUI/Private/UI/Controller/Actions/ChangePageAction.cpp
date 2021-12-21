#include "UI/Controller/Actions/ChangePageAction.h"
#include "UI/Controller/FairyController.h"
#include "Utils/ByteBuffer.h"

void FChangePageAction::Setup(FairyGUI::FByteBuffer* Buffer)
{
	FControllerAction::Setup(Buffer);

	ObjectID = Buffer->ReadFNameFromCache();
	ControllerName = Buffer->ReadFNameFromCache();
	TargetPage = Buffer->ReadFNameFromCache();
}

void FChangePageAction::Enter(UFairyController* Controller)
{
	if (ControllerName.IsNone())
	{
		return;
	}

	UFairyComponent* gcom = nullptr;
	if (!ObjectID.IsNone())
	{
		gcom = Cast<UFairyComponent>(Cast<UFairyComponent>(Controller->GetOuter())->GetChildByID(ObjectID.ToString()));
	}
	else
	{
		gcom = Cast<UFairyComponent>(Controller->GetOuter());
	}

	if (gcom != nullptr)
	{
		UFairyController* cc = gcom->GetController(ControllerName);
		if (cc != nullptr && cc != Controller && !cc->bChanging)
		{
			if (TargetPage.Compare("~1") == 0)
			{
				if (Controller->GetSelectedIndex() < cc->GetPageCount())
				{
					cc->SetSelectedIndex(Controller->GetSelectedIndex());
				}
			}
			else if (TargetPage.Compare("~2") == 0)
			{
				cc->SetSelectedPage(Controller->GetSelectedPage());
			}
			else
			{
				cc->SetSelectedPageID(TargetPage);
			}
		}
	}
}

void FChangePageAction::Leave(UFairyController* Controller)
{
}