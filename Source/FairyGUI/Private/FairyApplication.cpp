#include "FairyApplication.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SGameLayerManager.h"
#include "UI/FairyRoot.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "UI/UIObjectFactory.h"
#include "Widgets/NTexture.h"
#if WITH_EDITOR
#include "Editor.h"
#endif

UFairyApplication* UFairyApplication::Instance = nullptr;

UFairyApplication::FTouchInfo::FTouchInfo() :
	UserIndex(0),
	PointerIndex(0),
	bDown(false),
	DownPosition(0, 0),
	bClickCancelled(false),
	ClickCount(0)
{
}

void UFairyApplication::FInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool UFairyApplication::FInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (!UFairyApplication::IsStarted())
	{
		return false;
	}

	UFairyApplication::Get()->PreviewDownEvent(MouseEvent);
	return false;
}

bool UFairyApplication::FInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (!UFairyApplication::IsStarted())
	{
		return false;
	}

	UFairyApplication::Get()->PreviewUpEvent(MouseEvent);
	return false;
}

bool UFairyApplication::FInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (!UFairyApplication::IsStarted())
	{
		return false;
	}

	UFairyApplication::Get()->PreviewMoveEvent(MouseEvent);
	return false;
}

UFairyApplication::UFairyApplication() :
	bSoundEnabled(true),
	SoundVolumeScale(1)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UIFairyApplicaiton::UFairyApplication(...)"));
		LastTouch = new FTouchInfo();
		Touches.Add(LastTouch);
	}
}

UFairyApplication::~UFairyApplication()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UIFairyApplicaiton::~UFairyApplication(...)"));
	}
}

UFairyApplication* UFairyApplication::Get()
{
	if (Instance != nullptr)
	{
		return Instance;
	}


	Instance = NewObject<UFairyApplication>();
	Instance->AddToRoot();
	Instance->OnCreate();
	return Instance;
}

void UFairyApplication::Destroy()
{
	if (Instance != nullptr)
	{
		Instance->OnDestroy();
		Instance->RemoveFromRoot();
	}
	Instance = nullptr;
}

UFairyControllerMgr* UFairyApplication::GetControllerMgr()
{
	if (!FairyControllerMgr)
	{
		FairyControllerMgr = NewObject<UFairyControllerMgr>(this);
	}
	return FairyControllerMgr;
}

UFairyTweenMgr* UFairyApplication::GetTweenMgr()
{
	if (!FairyTweenMgr)
	{
		FairyTweenMgr = NewObject<UFairyTweenMgr>(this);
	}
	return FairyTweenMgr;
}

void UFairyApplication::AddUIRoot(UObject* WorldContextObject)
{
	//UE_LOG(LogTemp, Warning, TEXT("UFairyApplication::AddUIRoot(...)"));
	UWorld* World = WorldContextObject->GetWorld();
	if (World && World->IsGameWorld())
	{
		UGameViewportClient* ViewportClient = World->GetGameViewport();
		this->DPIScale = ViewportClient->GetDPIScale();

		UFairyRoot* NewUIRoot = NewObject<UFairyRoot>(WorldContextObject);
		//NewUIRoot->MakeSlateWidget();
		NewUIRoot->AddToViewport();
		UIRoots.Add(World, NewUIRoot);

		UDragDropManager* NewDragDropManager = NewObject<UDragDropManager>(WorldContextObject);
		DragDropManagers.Add(World, NewDragDropManager);
	}
}

UFairyRoot* UFairyApplication::GetUIRoot(UObject* WorldContextObject)
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyApplication::GetUIRoot(...)"));
	UWorld* World = WorldContextObject->GetWorld();
	if (World && World->IsGameWorld())
	{
		return UIRoots[World];
	}
	else
	{
		return nullptr;
	}
}

void UFairyApplication::RemoveUIRoot(UObject* WorldContextObject)
{
	UE_LOG(LogTemp, Warning, TEXT("UFairyApplication::RemoveUIRoot(...)"));
	UWorld* World = WorldContextObject->GetWorld();
	if (World && World->IsGameWorld() && UIRoots.Num() != 0)
	{
		UFairyRoot* TargetUIRoot = UIRoots[World];
		TargetUIRoot->RemoveFromViewport();

		UIRoots.Remove(World);
		DragDropManagers.Remove(World);
	}
}


void UFairyApplication::OnCreate()
{
	InputProcessor = MakeShareable(new FInputProcessor());
	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);

	PostTickDelegateHandle = FSlateApplication::Get().OnPostTick().AddUObject(this, &UFairyApplication::OnSlatePostTick);
}

void UFairyApplication::OnDestroy()
{
	FUIObjectFactory::PackageItemExtensions.Reset();
	FUIObjectFactory::LoaderCreator.Unbind();

	UNTexture::DestroyWhiteTexture();
	//UFairyConfig::Config = UFairyConfig(); //Reset Configuration to default values

	if (InputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

	if (PostTickDelegateHandle.IsValid())
	{
		FSlateApplication::Get().OnPostTick().Remove(PostTickDelegateHandle);
	}
}

void UFairyApplication::CallAfterSlateTick(FSimpleDelegate Callback)
{
	PostTickMulticastDelegate.Add(Callback);
}

void UFairyApplication::OnSlatePostTick(float DeltaTime)
{
	if (PostTickMulticastDelegate.IsBound())
	{
		PostTickMulticastDelegate.Broadcast();
		PostTickMulticastDelegate.Clear();
	}

	if (bNeedCheckPopups)
	{
		bNeedCheckPopups = false;
		for (auto It = UIRoots.CreateConstIterator(); It; ++It)
		{
			if (It.Value())
			{
				It.Value()->CheckPopups(nullptr);
			}
		}
	}
}

FVector2D UFairyApplication::GetTouchPosition(int32 InUserIndex, int32 InPointerIndex)
{
	FTouchInfo* TouchInfo = GetTouchInfo(InUserIndex, InPointerIndex);
	if (TouchInfo != nullptr)
	{
		return TouchInfo->Event.GetScreenSpacePosition();
	}
	else
	{
		return FVector2D::ZeroVector;
	}
}

int32 UFairyApplication::GetTouchCount() const
{
	int32 Count = 0;
	for (auto &it : Touches)
	{
		if (it.bDown)
		{
			Count++;
		}
	}

	return Count;
}

UFairyObject* UFairyApplication::GetObjectUnderPoint(const FVector2D& ScreenspacePosition)
{
	TSharedRef<SWindow> TopWindow = FSlateApplication::Get().GetActiveTopLevelWindow().ToSharedRef();
	TArray<TSharedRef<SWindow>> Windows;

	Windows.Add(TopWindow);
	FWidgetPath WidgetPath = FSlateApplication::Get().LocateWindowUnderMouse(ScreenspacePosition, Windows, false);

	if (WidgetPath.IsValid())
	{
		return GetWidgetGObject(WidgetPath.GetLastWidget());
	}
	else
	{
		return nullptr;
	}
}

void UFairyApplication::CancelClick(int32 InUserIndex, int32 InPointerIndex)
{
	FTouchInfo* TouchInfo = GetTouchInfo(InUserIndex, InPointerIndex);
	if (TouchInfo != nullptr)
	{
		TouchInfo->bClickCancelled = true;
	}
}

void UFairyApplication::PlaySound(const FString& URL, float VolumnScale)
{
	if (!bSoundEnabled)
	{
		return;
	}

	TSharedPtr<FFairyPackageItem> SoundItem = UFairyPackageMgr::Get()->GetPackageItemByURL(URL);
	if (SoundItem.IsValid())
	{
		SoundItem->Load();
		FSlateApplication::Get().PlaySound(SoundItem->Sound);
	}
}

void UFairyApplication::SetSoundEnabled(bool bEnabled)
{
	bSoundEnabled = bEnabled;
}

void UFairyApplication::SetSoundVolumeScale(float VolumnScale)
{
	SoundVolumeScale = VolumnScale;
}

bool UFairyApplication::DispatchEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator, const FNVariant& Data)
{
	UFairyObject* Obj = GetWidgetGObject(Initiator);
	if (Obj == nullptr)
	{
		return false;
	}

	UEventContext* Context = BorrowEventContext();
	Context->Type = EventType;
	Context->Initiator = Obj;
	Context->Sender = Obj;
	Context->Data = Data;

	Context->Sender->InvokeEventDelegate(Context);

	ReturnEventContext(Context);

	return Context->bDefaultPrevented;
}

void UFairyApplication::BubbleEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator, const FNVariant& Data)
{
	TArray<UFairyObject*> CallChain;
	GetFairyObjectPath(Initiator, CallChain);

	if (CallChain.Num() == 0)
	{
		return;
	}

	InternalBubbleEvent(EventType, CallChain, Data);
}

void UFairyApplication::InternalBubbleEvent(const FName& EventType, const TArray<UFairyObject*>& CallChain, const FNVariant& Data)
{
	UEventContext* Context = BorrowEventContext();
	Context->Type = EventType;
	Context->Initiator = CallChain[0];
	Context->Data = Data;

	for (auto& it : CallChain)
	{
		Context->Sender = it;
		it->InvokeEventDelegate(Context);

		if (Context->bMouseCaptured)
		{
			Context->bMouseCaptured = false;
			AddMouseCaptor(Context->GetUserIndex(), (int32)Context->GetPointerIndex(), it);
		}

		if (Context->IsPropagationStopped())
		{
			break;
		}
	}

	ReturnEventContext(Context);
}

void UFairyApplication::BroadcastEvent(const FName& EventType, const TSharedRef<SWidget>& Initiator, const FNVariant& Data)
{
	TArray<UFairyObject*> CallChain;
	GetDescendants(Initiator, CallChain);
	if (CallChain.Num() == 0)
	{
		return;
	}

	UEventContext* Context = BorrowEventContext();
	Context->Type = EventType;
	Context->Data = Data;

	for (auto& it : CallChain)
	{
		Context->Sender = it;
		Context->Initiator = it;
		it->InvokeEventDelegate(Context);
	}

	ReturnEventContext(Context);
}

UFairyObject* UFairyApplication::GetWidgetGObject(const TSharedPtr<SWidget>& InWidget)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<SWidget> Ptr = InWidget;
	while (Ptr.IsValid())
	{
		if (Ptr->GetTag() == SDisplayObject::SDisplayObjectTag)
		{
			const TWeakObjectPtr<UFairyObject> ObjPtr = StaticCastSharedPtr<SDisplayObject>(Ptr)->GetFairyObject();
			if (ObjPtr.IsValid())
			{
				return ObjPtr.Get();
			}
		}

		Ptr = Ptr->GetParentWidget();
	}

	return nullptr;
}

void UFairyApplication::GetFairyObjectPath(const TSharedRef<SWidget>& InWidget, TArray<UFairyObject*>& OutArray)
{
	TSharedPtr<SWidget> Ptr = InWidget;
	TWeakObjectPtr<UFairyObject> ObjPtr = nullptr;

	// 1. find the UFairyObject that handle the widget.
	while (Ptr.IsValid())
	{
		if (Ptr->GetTag() == SDisplayObject::SDisplayObjectTag)
		{
			ObjPtr = StaticCastSharedPtr<SDisplayObject>(Ptr)->GetFairyObject();
			break;
		}
		Ptr = Ptr->GetParentWidget();
	}

	// 2. find all UFairyObject
	while (ObjPtr.IsValid() && ObjPtr->IsA<UFairyObject>())
	{
		OutArray.Add(ObjPtr.Get());
		ObjPtr = ObjPtr->GetParent();
	}
}

void UFairyApplication::GetDescendants(const TSharedRef<SWidget>& InWidget, TArray<UFairyObject*>& OutArray)
{
	TSharedPtr<SWidget> Ptr = InWidget;
	if (InWidget->GetTag() == SDisplayObject::SDisplayObjectTag)
	{
		const TWeakObjectPtr<UFairyObject> ObjPtr = StaticCastSharedPtr<SDisplayObject>(Ptr)->GetFairyObject();
		if (ObjPtr.IsValid())
		{
			OutArray.Add(ObjPtr.Get());
		}
	}

	FChildren* Children = InWidget->GetChildren();
	for (int32 SlotIdx = 0; SlotIdx < Children->Num(); ++SlotIdx)
	{
		GetDescendants(Children->GetChildAt(SlotIdx), OutArray);
	}
}

UEventContext* UFairyApplication::BorrowEventContext()
{
	UEventContext* Context = nullptr;
	if (EventContextPool.Num() > 0)
	{
		Context = EventContextPool.Pop();
	}
	else
	{
		Context = NewObject<UEventContext>(this);
	}
	Context->PointerEvent = &LastTouch->Event;
	//Context->KeyEvent = &LastKeyEvent;

	verify(Context != nullptr);
	return Context;
}

void UFairyApplication::ReturnEventContext(UEventContext* Context)
{
	Context->Reset();
	EventContextPool.Add(Context);
}

void UFairyApplication::AddMouseCaptor(int32 InUserIndex, int32 InPointerIndex, UFairyObject* InTarget)
{
	FTouchInfo* TouchInfo = GetTouchInfo(InUserIndex, InPointerIndex);
	if (TouchInfo != nullptr && !TouchInfo->MouseCaptors.Contains(InTarget))
	{
		TouchInfo->MouseCaptors.Add(InTarget);
	}
}

void UFairyApplication::RemoveMouseCaptor(int32 InUserIndex, int32 InPointerIndex, UFairyObject* InTarget)
{
	FTouchInfo* TouchInfo = GetTouchInfo(InUserIndex, InPointerIndex);
	if (TouchInfo != nullptr)
	{
		TouchInfo->MouseCaptors.Remove(InTarget);
	}
}

bool UFairyApplication::HasMouseCaptor(int32 InUserIndex, int32 InPointerIndex)
{
	FTouchInfo* TouchInfo = GetTouchInfo(InUserIndex, InPointerIndex);
	return TouchInfo != nullptr && TouchInfo->MouseCaptors.Num() > 0;
}

UFairyApplication::FTouchInfo* UFairyApplication::GetTouchInfo(const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = nullptr;
	for (auto &it : Touches)
	{
		if (it.UserIndex == MouseEvent.GetUserIndex() && it.PointerIndex == (int32)MouseEvent.GetPointerIndex())
		{
			TouchInfo = &it;
			break;
		}
	}

	if (TouchInfo == nullptr)
	{
		TouchInfo = new FTouchInfo();
		TouchInfo->UserIndex = MouseEvent.GetUserIndex();
		TouchInfo->PointerIndex = (int32)MouseEvent.GetPointerIndex();

		Touches.Add(TouchInfo);
	}

	LastTouch = TouchInfo;
	return TouchInfo;
}

UFairyApplication::FTouchInfo* UFairyApplication::GetTouchInfo(int32 InUserIndex, int32 InPointerIndex)
{
	if (InUserIndex == -1 && InPointerIndex == -1)
	{
		return LastTouch;
	}

	for (auto &it : Touches)
	{
		if (it.UserIndex == InUserIndex && it.PointerIndex == InPointerIndex)
		{
			return &it;
		}
	}
	return nullptr;
}

void UFairyApplication::PreviewDownEvent(const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	TouchInfo->Event = MouseEvent;
	TouchInfo->bDown = true;
	TouchInfo->DownPosition = MouseEvent.GetScreenSpacePosition();
	TouchInfo->bClickCancelled = false;
	TouchInfo->ClickCount = 1;
	TouchInfo->MouseCaptors.Reset();
	TouchInfo->bToClearCaptors = false;
	TouchInfo->DownPath.Reset();

//#if WITH_EDITOR
//	const FVector2D ScreenSpacePos = MouseEvent.GetScreenSpacePosition();
//	UE_LOG(LogFairyGUI, Log, TEXT("---> UFairyApplication::PreviewDownEvent(...), ScreenSpace, X=%f, Y=%f"), ScreenSpacePos.X, ScreenSpacePos.Y);
//#endif

	bNeedCheckPopups = true;
}

void UFairyApplication::PreviewUpEvent(const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	TouchInfo->Event = MouseEvent;
	TouchInfo->bDown = false;
	TouchInfo->bToClearCaptors = true;

	if (TouchInfo->MouseCaptors.Num() > 0)
	{
		int32 cnt = TouchInfo->MouseCaptors.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			auto& Captor = TouchInfo->MouseCaptors[i];
			if (Captor.IsValid() && Captor->OnStage())
			{
				DispatchEvent(FFairyEventNames::TouchEnd, Captor->GetDisplayObject());
			}
		}
	}
}

void UFairyApplication::PreviewMoveEvent(const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	TouchInfo->Event = MouseEvent;

	if ((TouchInfo->DownPosition - MouseEvent.GetScreenSpacePosition()).GetAbsMax() > 50)
	{
		TouchInfo->bClickCancelled = true;
	}

	if (!TouchInfo->bToClearCaptors && TouchInfo->MouseCaptors.Num() > 0)
	{
		int32 cnt = TouchInfo->MouseCaptors.Num();
		for (int32 i = 0; i < cnt; i++)
		{
			auto& Captor = TouchInfo->MouseCaptors[i];
			if (Captor.IsValid() && Captor->OnStage())
			{
				DispatchEvent(FFairyEventNames::TouchMove, Captor->GetDisplayObject());
			}
		}
	}
}

FReply UFairyApplication::OnWidgetMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("---> UFairyApplication::OnWidgetMouseButtonDown(...)"));
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	TSharedPtr<SWidget> Ptr = Widget;
	while (Ptr.IsValid())
	{
		TouchInfo->DownPath.Add(Ptr);
		Ptr = Ptr->GetParentWidget();
	}
	bNeedCheckPopups = false;
	BubbleEvent(FFairyEventNames::TouchBegin, Widget);
	return FReply::Handled();
}

FReply UFairyApplication::OnWidgetMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("---> UFairyApplication::OnWidgetMouseButtonUp(...)"));
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	if (TouchInfo == nullptr)
	{
		return FReply::Handled();
	}

	TArray<UFairyObject*> CallChain;
	GetFairyObjectPath(Widget, CallChain);
	if (CallChain.Num() > 0)
	{
		for (auto& it : TouchInfo->MouseCaptors)
		{
			if (it.IsValid())
			{
				CallChain.RemoveSingle(it.Get());
			}
		}

		if (CallChain.Num() > 0)
		{
			InternalBubbleEvent(FFairyEventNames::TouchEnd, CallChain, FNVariant::Null);
		}
	}
	TouchInfo->MouseCaptors.Reset();

	if (!TouchInfo->bClickCancelled)
	{
		TSharedPtr<SWidget> Ptr = Widget;
		while (Ptr.IsValid())
		{
			if (TouchInfo->DownPath.Contains(Ptr))
			{
				BubbleEvent(FFairyEventNames::Click, Ptr.ToSharedRef());
				break;
			}

			Ptr = Ptr->GetParentWidget();
		}
	}
	TouchInfo->DownPath.Reset();

	return FReply::Handled();
}

FReply UFairyApplication::OnWidgetMouseMove(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

FReply UFairyApplication::OnWidgetMouseButtonDoubleClick(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("---> UFairyApplication::OnWidgetMouseButtonDoubleClick(...)"));
	return OnWidgetMouseButtonDown(Widget, MyGeometry, MouseEvent);
	//return FReply::Handled();
}

void UFairyApplication::OnWidgetMouseEnter(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	DispatchEvent(FFairyEventNames::RollOver, Widget);
}

void UFairyApplication::OnWidgetMouseLeave(const TSharedRef<SWidget>& Widget, const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	DispatchEvent(FFairyEventNames::RollOut, Widget);
}

FReply UFairyApplication::OnWidgetMouseWheel(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FTouchInfo* TouchInfo = GetTouchInfo(MouseEvent);
	TouchInfo->Event = MouseEvent;

	BubbleEvent(FFairyEventNames::MouseWheel, Widget);

	return FReply::Handled();
}
