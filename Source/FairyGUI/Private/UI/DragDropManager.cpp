#include "UI/DragDropManager.h"
#include "FairyApplication.h"
#include "UI/FairyRoot.h"

//UDragDropManager* UDragDropManager::Get()
//{
//    return UFairyApplication::Get()->DragDropManager;
//}

UDragDropManager::UDragDropManager()
{
	
}

UDragDropManager::~UDragDropManager()
{
}

void UDragDropManager::Init(UObject* WorldContextObject)
{
	Agent = (UGLoader*)FUIObjectFactory::NewObject(WorldContextObject, EObjectType::Loader);
	Agent->SetName(TEXT("DragDropAgent"));
	Agent->SetTouchable(false);
	Agent->SetDraggable(true);
	//Agent->SetSize(FVector2D(100, 100));
	//Agent->SetPivot(FVector2D(.5f, .5f), true);
	Agent->SetAlign(EHAlignType::Center);
	Agent->SetVerticalAlign(EVAlignType::Middle);
	Agent->SetSortingOrder(INT_MAX);
	Agent->On(FFairyEventNames::DragEnd).AddUObject(this, &UDragDropManager::OnDragEnd);
}

void UDragDropManager::StartDrag(const FString& InIcon, const FNVariant& InUserData, int32 InUserIndex, int32 InPointerIndex)
{
	if (Agent->GetParent() != nullptr)
		return;

	UFairyRoot* TargetUIRoot = UFairyApplication::Get()->GetUIRoot(this);
	UserData = InUserData;
	Agent->SetURL(InIcon);
	TargetUIRoot->AddChild(Agent);
	FVector2D pt = TargetUIRoot->GlobalToLocal(UFairyApplication::Get()->GetTouchPosition(InUserIndex, InPointerIndex));
	Agent->SetPosition(pt);
	UFairyApplication::Get()->CallAfterSlateTick(FSimpleDelegate::CreateUObject(this, &UDragDropManager::DelayStartDrag, InUserIndex, InPointerIndex));
}

void UDragDropManager::DelayStartDrag(int32 InUserIndex, int32 InPointerIndex)
{
	if (Agent->GetParent() != nullptr)
		Agent->StartDrag(InUserIndex, InPointerIndex);
}

void UDragDropManager::Cancel()
{
	if (Agent->GetParent() != nullptr)
	{
		Agent->StopDrag();
		Agent->RemoveFromParent();
		UserData.Reset();
	}
}

void UDragDropManager::OnDragEnd(UEventContext* Context)
{
	if (Agent->GetParent() == nullptr) //canceled
		return;

	Agent->RemoveFromParent();

	UFairyObject* obj = UFairyApplication::Get()->GetObjectUnderPoint(Context->GetPointerPosition());
	while (obj != nullptr)
	{
		if (obj->IsA<UFairyComponent>())
		{
			if (obj->HasEventListener(FFairyEventNames::Drop))
			{
				obj->DispatchEvent(FFairyEventNames::Drop, UserData);
				return;
			}
		}

		obj = obj->GetParent();
	}
}