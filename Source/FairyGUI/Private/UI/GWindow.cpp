#include "UI/GWindow.h"
#include "UI/GGraph.h"
#include "UI/FairyRoot.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "FairyApplication.h"

UGWindow::UGWindow()
{
    //bBringToFontOnClick = UFairyConfig::Config->BringWindowToFrontOnClick;

    On(FFairyEventNames::AddedToStage).AddUObject(this, &UGWindow::OnAddedToStageHandler);
    On(FFairyEventNames::RemovedFromStage).AddUObject(this, &UGWindow::OnRemovedFromStageHandler);
    On(FFairyEventNames::TouchBegin).AddUObject(this, &UGWindow::OnTouchBeginHandler);
}

UGWindow::~UGWindow()
{
}

void UGWindow::SetContentPane(UFairyComponent* Obj)
{
    if (ContentPane != Obj)
    {
        if (ContentPane != nullptr)
        {
            RemoveChild(ContentPane);

            FrameObject = nullptr;
        }
        ContentPane = Obj;
        if (ContentPane != nullptr)
        {
            AddChild(ContentPane);
            SetSize(ContentPane->GetSize());
            ContentPane->AddRelation(this, ERelationType::Size);
            FrameObject = Cast<UFairyComponent>(ContentPane->GetChild("frame"));
            if (FrameObject != nullptr)
            {
                SetCloseButton(FrameObject->GetChild("closeButton"));
                SetDragArea(FrameObject->GetChild("dragArea"));
                SetContentArea(FrameObject->GetChild("contentArea"));
            }
        }
        else
            FrameObject = nullptr;
    }
}

void UGWindow::SetCloseButton(UFairyObject * Obj)
{
    if (CloseButton != Obj)
    {
        if (CloseButton != nullptr)
            CloseButton->OnClick.RemoveDynamic(this, &UGWindow::CloseEventHandler);
        CloseButton = Obj;
        if (CloseButton != nullptr)
            CloseButton->OnClick.AddUniqueDynamic(this, &UGWindow::CloseEventHandler);
    }
}

void UGWindow::SetDragArea(UFairyObject * Obj)
{
    if (DragArea != Obj)
    {
        if (DragArea != nullptr)
        {
            DragArea->SetDraggable(false);
            DragArea->OnDragStart.RemoveDynamic(this, &UGWindow::OnDragStartHandler);
        }

        DragArea = Obj;
        if (DragArea != nullptr)
        {
            UGGraph* DragGraph;
            if ((DragGraph = Cast<UGGraph>(DragArea)) != nullptr && DragGraph->IsEmpty())
                DragGraph->DrawRect(0, FColor::Transparent, FColor::Transparent);

            DragArea->SetDraggable(true);
            DragArea->OnDragStart.AddUniqueDynamic(this, &UGWindow::OnDragStartHandler);
        }
    }
}

void UGWindow::Show()
{
    UFairyApplication::Get()->GetUIRoot(this)->ShowWindow(this);
}

void UGWindow::Hide()
{
    if (IsShowing())
        DoHideAnimation();
}

void UGWindow::HideImmediately()
{
    UFairyApplication::Get()->GetUIRoot(this)->HideWindowImmediately(this);
}

void UGWindow::ToggleStatus()
{
    if (IsTop())
        Hide();
    else
        Show();
}

void UGWindow::BringToFront()
{
    UFairyApplication::Get()->GetUIRoot(this)->BringToFront(this);
}

bool UGWindow::IsTop() const
{
    return Parent != nullptr && Parent->GetChildIndex(this) == Parent->NumChildren() - 1;
}

void UGWindow::ShowModalWait(int32 InRequestingCmd)
{
    if (InRequestingCmd != 0)
        RequestingCmd = InRequestingCmd;

    if (!UFairyConfig::Config->WindowModalWaiting.IsEmpty())
    {
        if (ModalWaitPane == nullptr)
            ModalWaitPane = UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), FName(UFairyConfig::Config->WindowModalWaiting));

        LayoutModalWaitPane();

        AddChild(ModalWaitPane);
    }
}

void UGWindow::LayoutModalWaitPane()
{
    if (ContentArea != nullptr)
    {
        FVector2D pt = FrameObject->LocalToGlobal(FVector2D::ZeroVector);
        pt = GlobalToLocal(pt);
        ModalWaitPane->SetPosition(pt + ContentArea->GetPosition());
        ModalWaitPane->SetSize(ContentArea->GetSize());
    }
    else
        ModalWaitPane->SetSize(Size);
}

bool UGWindow::CloseModalWait(int32 InRequestingCmd)
{
    if (InRequestingCmd != 0)
    {
        if (RequestingCmd != InRequestingCmd)
            return false;
    }
    RequestingCmd = 0;

    if (ModalWaitPane != nullptr && ModalWaitPane->GetParent() != nullptr)
        RemoveChild(ModalWaitPane);

    return true;
}

void UGWindow::Init()
{
    if (bInited || bLoading)
        return;

    if (UISources.Num() > 0)
    {
        bLoading = false;
        int32 cnt = UISources.Num();
        for (int32 i = 0; i < cnt; i++)
        {
            const TSharedPtr<IUISource>& lib = UISources[i];
            if (!lib->IsLoaded())
            {
                lib->Load(FSimpleDelegate::CreateUObject(this, &UGWindow::OnUILoadComplete));
                bLoading = true;
            }
        }

        if (!bLoading)
            InternalInit();
    }
    else
        InternalInit();
}

void UGWindow::InternalInit()
{
    bInited = true;
    OnInit();

    if (IsShowing())
        DoShowAnimation();
}

void UGWindow::AddUISource(TSharedPtr<IUISource> UISource)
{
    UISources.Add(UISource);
}

void UGWindow::OnInit()
{
    InitCallback.ExecuteIfBound();
}

void UGWindow::OnShown()
{
    ShownCallback.ExecuteIfBound();
}

void UGWindow::OnHide()
{
    HideCallback.ExecuteIfBound();
}

void UGWindow::DoShowAnimation()
{
    if (ShowingCallback.IsBound())
        ShowingCallback.Execute();
    else
        OnShown();
}

void UGWindow::DoHideAnimation()
{
    if (HidingCallback.IsBound())
        HidingCallback.Execute();
    else
        HideImmediately();
}

void UGWindow::CloseEventHandler(UEventContext * Context)
{
    Hide();
}

void UGWindow::OnUILoadComplete()
{
    int32 cnt = UISources.Num();
    for (int32 i = 0; i < cnt; i++)
    {
        const TSharedPtr<IUISource>& lib = UISources[i];
        if (!lib->IsLoaded())
            return;
    }

    bLoading = false;
    InternalInit();
}

void UGWindow::OnAddedToStageHandler(UEventContext * Context)
{
    if (!bInited)
        Init();
    else
        DoShowAnimation();
}

void UGWindow::OnRemovedFromStageHandler(UEventContext * Context)
{
    CloseModalWait();
    OnHide();
}

void UGWindow::OnTouchBeginHandler(UEventContext * Context)
{
    if (IsShowing() && bBringToFontOnClick)
    {
        BringToFront();
    }
}

void UGWindow::OnDragStartHandler(UEventContext * Context)
{
    Context->PreventDefault();

    StartDrag(Context->GetUserIndex(), Context->GetPointerIndex());
}