#include "UI/FairyRoot.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Slate.h"
#include "FairyApplication.h"
#include "UI/GWindow.h"
#include "UI/GGraph.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "Widgets/SContainer.h"
#include "Widgets/Layout/SConstraintCanvas.h"

int32 UFairyRoot::ContentScaleLevel = 0;

class SRootContainer : public SContainer
{
public:
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
};

void SRootContainer::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
    const FVector2D& LocalSize = AllottedGeometry.GetLocalSize();
    //const FVector2D& AbsoluteSize = AllottedGeometry.GetAbsoluteSize();
    TWeakObjectPtr<UFairyObject> ObjectPtr = GetFairyObject();
    if (ObjectPtr.IsValid() && (LocalSize != ObjectPtr->GetSize()))
    {
        UE_LOG(LogFairyGUI, Log, TEXT("FairyRoot Widget Size:(%f, %f)"), LocalSize.X, LocalSize.Y);
        ObjectPtr->SetSize(LocalSize);
    }

    SContainer::OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}

UFairyRoot::UFairyRoot() :
    Super()
{
    UE_LOG(LogTemp, Warning, TEXT("UFairyRoot::UFairyRoot()"))
}

UFairyRoot::~UFairyRoot()
{
    UE_LOG(LogTemp, Warning, TEXT("UFairyRoot::~UFairyRoot()"))
}

void UFairyRoot::ReleaseSlateResources(bool bReleaseChildren)
{
    if (FullScreenWidget.IsValid())
    {
        FullScreenWidget.Reset();
    }
    Super::ReleaseSlateResources(bReleaseChildren);
}

void UFairyRoot::BeginDestroy()
{
    Super::BeginDestroy();
    UE_LOG(LogTemp, Warning, TEXT("UFairyRoot::BeginDestroy(...)"));
}

void UFairyRoot::AddToViewport()
{
    UWorld* World = GetWorld();
    if (World && World->IsGameWorld())
    {
        UGameViewportClient* ViewportClient = World->GetGameViewport();
        //TSharedRef<SConstraintCanvas> FullScreenCanvas = SNew(SConstraintCanvas);
        
        TSharedRef<SRootContainer> NewRootContainer = SNew(SRootContainer).GObject(this);
        Container = NewRootContainer;
        DisplayObject = NewRootContainer;
        FullScreenWidget = NewRootContainer;


        NewRootContainer->SetOpaque(false);
        ViewportClient->AddViewportWidgetContent(NewRootContainer, 100);
    }
}

void UFairyRoot::RemoveFromViewport()
{
    if (!HasAnyFlags(RF_BeginDestroyed))
    {
        if (FullScreenWidget.IsValid())
        {
            TSharedPtr<SWidget> WidgetHost = FullScreenWidget;

            // If this is a game world remove the widget from the current world's viewport.
            UWorld* World = GetWorld();
            if (World && World->IsGameWorld())
            {
                if (UGameViewportClient* ViewportClient = World->GetGameViewport())
                {
                    TSharedRef<SWidget> WidgetHostRef = WidgetHost.ToSharedRef();

                    ViewportClient->RemoveViewportWidgetContent(WidgetHostRef);
                }
            }
            FullScreenWidget.Reset();
        }
    }
}

void UFairyRoot::ShowWindow(UGWindow* Window)
{
    AddChild(Window);
    AdjustModalLayer();
}

void UFairyRoot::HideWindow(UGWindow* Window)
{
    Window->Hide();
}

void UFairyRoot::HideWindowImmediately(UGWindow* Window)
{
    if (Window->GetParent() == this)
    {
        RemoveChild(Window);
    }

    AdjustModalLayer();
}

void UFairyRoot::BringToFront(UGWindow* Window)
{
    int32 cnt = NumChildren();
    int32 i;
    if (ModalLayer->GetParent() != nullptr && !Window->IsModal())
    {
        i = GetChildIndex(ModalLayer) - 1;
    }
    else
    {
        i = cnt - 1;
    }

    for (; i >= 0; i--)
    {
        UFairyObject* g = GetChildAt(i);
        if (g == Window)
        {
            return;
        }

        if (g->IsA<UGWindow>())
        {
            break;
        }
    }

    if (i >= 0)
    {
        SetChildIndex(Window, i);
    }
}

void UFairyRoot::CloseAllExceptModals()
{
    TArray<UFairyObject*> map;
    map.Append(Children);

    for (const auto& child : map)
    {
        if (child->IsA<UGWindow>() && !((UGWindow*)child)->IsModal())
        {
            HideWindowImmediately((UGWindow*)child);
        }
    }
}

void UFairyRoot::CloseAllWindows()
{
    TArray<UFairyObject*> map;
    map.Append(Children);

    for (const auto& child : map)
    {
        if (child->IsA<UGWindow>())
        {
            HideWindowImmediately((UGWindow*)child);
        }
    }
}

UGWindow* UFairyRoot::GetTopWindow() const
{
    int32 cnt = NumChildren();
    for (int32 i = cnt - 1; i >= 0; i--)
    {
        UFairyObject* child = GetChildAt(i);
        if (child->IsA<UGWindow>())
        {
            return (UGWindow*)child;
        }
    }

    return nullptr;
}

UGGraph* UFairyRoot::GetModalLayer()
{
    if (ModalLayer == nullptr)
    {
        CreateModalLayer();
    }

    return ModalLayer;
}

void UFairyRoot::CreateModalLayer()
{
    ModalLayer = NewObject<UGGraph>();
    ModalLayer->SetSize(Size);
    ModalLayer->DrawRect(0, FColor::White, UFairyConfig::Config->ModalLayerColor);
    ModalLayer->AddRelation(this, ERelationType::Size);
}

void UFairyRoot::AdjustModalLayer()
{
    if (ModalLayer == nullptr)
    {
        CreateModalLayer();
    }

    int32 cnt = NumChildren();

    if (ModalWaitPane != nullptr && ModalWaitPane->GetParent() != nullptr)
    {
        SetChildIndex(ModalWaitPane, cnt - 1);
    }

    for (int32 i = cnt - 1; i >= 0; i--)
    {
        UFairyObject* child = GetChildAt(i);
        if (child->IsA<UGWindow>() && ((UGWindow*)child)->IsModal())
        {
            if (ModalLayer->GetParent() == nullptr)
            {
                AddChildAt(ModalLayer, i);
            }
            else
            {
                SetChildIndexBefore(ModalLayer, i);
            }
            return;
        }
    }

    if (ModalLayer->GetParent() != nullptr)
    {
        RemoveChild(ModalLayer);
    }
}

bool UFairyRoot::HasModalWindow() const
{
    return ModalLayer != nullptr && ModalLayer->GetParent() != nullptr;
}

void UFairyRoot::ShowModalWait()
{
    GetModalWaitingPane();
    if (ModalWaitPane)
    {
        AddChild(ModalWaitPane);
    }
}

void UFairyRoot::CloseModalWait()
{
    if (ModalWaitPane != nullptr && ModalWaitPane->GetParent() != nullptr)
    {
        RemoveChild(ModalWaitPane);
    }
}

UFairyObject* UFairyRoot::GetModalWaitingPane()
{
    if (!UFairyConfig::Config->GlobalModalWaiting.IsEmpty())
    {
        if (ModalWaitPane == nullptr)
        {
            ModalWaitPane = UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), UFairyConfig::Config->GlobalModalWaiting);
            ModalWaitPane->SetSortingOrder(INT_MAX);
        }

        ModalWaitPane->SetSize(GetSize());
        ModalWaitPane->AddRelation(this, ERelationType::Size);

        return ModalWaitPane;
    }
    else
    {
        return nullptr;
    }
}

bool UFairyRoot::IsModalWaiting() const
{
    return (ModalWaitPane != nullptr) && ModalWaitPane->OnStage();
}

void UFairyRoot::ShowPopup(UFairyObject* Popup, UFairyObject* AtObject, EPopupDirection Direction)
{
    if (PopupStack.Num() > 0)
    {
        HidePopup(Popup);
    }

    PopupStack.Add(Popup);

    if (AtObject != nullptr)
    {
        UFairyObject* p = AtObject;
        while (p != nullptr)
        {
            if (p->GetParent() == this)
            {
                if (Popup->GetSortingOrder() < p->GetSortingOrder())
                {
                    Popup->SetSortingOrder(p->GetSortingOrder());
                }
                break;
            }
            p = p->GetParent();
        }
    }

    AddChild(Popup);
    AdjustModalLayer();

    if (Popup->IsA<UGWindow>() && AtObject == nullptr && Direction == EPopupDirection::Auto)
    {
        return;
    }

    FVector2D pos = GetPoupPosition(Popup, AtObject, Direction);
    Popup->SetPosition(pos);
}

void UFairyRoot::TogglePopup(UFairyObject* Popup, UFairyObject* AtObject, EPopupDirection Direction)
{
    int32 Index;
    if (JustClosedPopups.Find(Popup, Index))
    {
        return;
    }

    ShowPopup(Popup, AtObject, Direction);
}

void UFairyRoot::HidePopup(UFairyObject* Popup)
{
    if (Popup != nullptr)
    {
        int32 k;
        if (PopupStack.Find(Popup, k))
        {
            for (int32 i = PopupStack.Num() - 1; i >= k; i--)
            {
                ClosePopup(PopupStack.Last());
                PopupStack.Pop();
            }
        }
    }
    else
    {
        for (const auto& it : PopupStack)
        {
            ClosePopup(it);
        }
        PopupStack.Reset();
    }
}

void UFairyRoot::ClosePopup(UFairyObject* Popup)
{
    if (Popup != nullptr && Popup->GetParent() != nullptr)
    {
        if (Popup->IsA<UGWindow>())
        {
            ((UGWindow*)Popup)->Hide();
        }
        else
        {
            RemoveChild(Popup);
        }
    }
}

void UFairyRoot::CheckPopups(SWidget* ClickTarget)
{
    if (IsPendingKill())
    {
        return;
    }

    JustClosedPopups.Reset();
    if (PopupStack.Num() > 0)
    {
        bool handled = false;
        SWidget* Ptr = ClickTarget;
        SWidget* Top = DisplayObject.Get();
        while (Ptr != Top && Ptr != nullptr)
        {
            if (Ptr->GetTag() == SDisplayObject::SDisplayObjectTag)
            {
                UFairyObject* Obj = static_cast<SDisplayObject*>(Ptr)->GetFairyObject().Get();

                int32 k;
                if (PopupStack.Find(Obj, k))
                {
                    for (int32 i = PopupStack.Num() - 1; i > k; i--)
                    {
                        ClosePopup(PopupStack.Pop());
                    }
                    handled = true;
                    break;
                }
            }
            Ptr = Ptr->GetParentWidget().Get();
        }

        if (!handled)
        {
            for (int32 i = PopupStack.Num() - 1; i >= 0; i--)
            {
                UFairyObject* popup = PopupStack[i];
                if (popup != nullptr)
                {
                    JustClosedPopups.Add(popup);
                    ClosePopup(popup);
                }
            }
            PopupStack.Reset();
        }
    }
}

bool UFairyRoot::HasAnyPopup() const
{
    return PopupStack.Num() > 0;
}

FVector2D UFairyRoot::GetPoupPosition(UFairyObject* Popup, UFairyObject* AtObject, EPopupDirection Direction)
{
    FVector2D pos;
    FVector2D size;
    if (AtObject != nullptr)
    {
        pos = AtObject->LocalToGlobal(FVector2D::ZeroVector);
        pos = this->GlobalToLocal(pos);
        size = AtObject->LocalToGlobal(AtObject->GetSize());
        size = this->GlobalToLocal(size);
        size -= pos;
    }
    else
    {
        pos = GlobalToLocal(UFairyApplication::Get()->GetTouchPosition());
    }
    FVector2D RetPosition;
    RetPosition.X = pos.X;
    if (RetPosition.X + Popup->GetWidth() > GetWidth())
    {
        RetPosition.X += size.X - Popup->GetWidth();

    }

    RetPosition.Y = pos.Y + size.Y;
    if ((Direction == EPopupDirection::Auto && RetPosition.Y + Popup->GetHeight() > GetHeight()) || Direction == EPopupDirection::Up)
    {
        RetPosition.Y = pos.Y - Popup->GetHeight() - 1;
        if (RetPosition.Y < 0)
        {
            RetPosition.Y = 0;
            RetPosition.X += size.X / 2;
        }
    }

    return RetPosition.RoundToVector();
}

void UFairyRoot::ShowTooltips(const FString& Text)
{
    if (DefaultTooltipWin == nullptr)
    {
        const FString& resourceURL = UFairyConfig::Config->TooltipsWin;
        if (resourceURL.IsEmpty())
        {
            UE_LOG(LogFairyGUI, Warning, TEXT("UIConfig.tooltipsWin not defined"));
            return;
        }

        DefaultTooltipWin = UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), resourceURL);
        DefaultTooltipWin->SetTouchable(false);
    }

    DefaultTooltipWin->SetText(Text);
    ShowTooltipsWin(DefaultTooltipWin);
}

void UFairyRoot::ShowTooltipsWin(UFairyObject* InTooltipWin)
{
    HideTooltips();

    TooltipWin = InTooltipWin;
    UWorld* World = GetWorld();
    World->GetTimerManager().SetTimer(ShowTooltipsTimerHandle, FTimerDelegate::CreateUObject(this, &UFairyRoot::DoShowTooltipsWin), 0.1f, false);
}

void UFairyRoot::DoShowTooltipsWin()
{
    if (TooltipWin == nullptr)
    {
        return;
    }

    FVector2D pt = UFairyApplication::Get()->GetTouchPosition();
    FVector2D Pos = pt + FVector2D(10, 20);

    Pos = GlobalToLocal(Pos);

    if (Pos.X + TooltipWin->GetWidth() > GetWidth())
    {
        Pos.X -= TooltipWin->GetWidth();
    }

    if (Pos.Y + TooltipWin->GetHeight() > GetHeight())
    {
        Pos.Y -= TooltipWin->GetHeight() - 1;
        if (Pos.Y < 0)
        {
            Pos.Y = 0;
        }
    }

    TooltipWin->SetPosition(Pos.RoundToVector());
    AddChild(TooltipWin);
}

void UFairyRoot::HideTooltips()
{
    if (TooltipWin != nullptr)
    {
        if (TooltipWin->GetParent() != nullptr)
        {
            RemoveChild(TooltipWin);
        }
        TooltipWin = nullptr;
    }
}

