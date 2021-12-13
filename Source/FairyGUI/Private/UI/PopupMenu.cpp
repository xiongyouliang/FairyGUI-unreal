#include "UI/PopupMenu.h"
#include "UI/FairyRoot.h"
#include "Package/FairyPackage.h"
#include "UI/Controller/GController.h"
#include "FairyApplication.h"

const FName UPopupMenu::ClickMenu("ClickMenu");

UPopupMenu* UPopupMenu::CreatePopupMenu(const FString& ResourceURL)
{
    UPopupMenu* Instance = NewObject<UPopupMenu>();
    Instance->Create(ResourceURL);
    return Instance;
}

UPopupMenu::UPopupMenu()
{
}

UPopupMenu::~UPopupMenu()
{
}

void UPopupMenu::Create(const FString& ResourceURL)
{
    FString url = ResourceURL;
    if (url.IsEmpty())
    {
        url = UFairyConfig::Config->PopupMenu;
        if (url.IsEmpty())
        {
            UE_LOG(LogFairyGUI, Warning, TEXT("UIConfig.PopupMenu not defined"));
            return;
        }
    }

    ContentPane = UFairyPackageMgr::Get()->CreateObjectFromURL(GetOuter(), FName(url))->As<UFairyComponent>();
    ContentPane->On(FFairyEventNames::AddedToStage).AddUObject(this, &UPopupMenu::OnAddedToStage);

    List = ContentPane->GetChild("list")->As<UFairyListView>();
    List->RemoveChildrenToPool();

    List->AddRelation(ContentPane, ERelationType::Width);
    List->RemoveRelation(ContentPane, ERelationType::Height);
    ContentPane->AddRelation(List, ERelationType::Height);

    List->On(FFairyEventNames::ClickItem).AddUObject(this, &UPopupMenu::OnClickItem);
}

UFairyButton* UPopupMenu::AddItem(const FString& Caption, FGUIEventDelegate Callback)
{
    UFairyButton* item = List->AddItemFromPool()->As<UFairyButton>();
    item->SetTitle(Caption);
    item->SetGrayed(false);
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(0);
    item->On(ClickMenu).Clear();
    if (Callback.IsBound())
        item->On(ClickMenu).Add(Callback);

    return item;
}

UFairyButton* UPopupMenu::AddItem(const FString& Caption, const FGUIEventDynDelegate& Callback)
{
    return AddItem(Caption, Callback.IsBound() ?
        FGUIEventDelegate::CreateUFunction(const_cast<UObject*>(Callback.GetUObject()), Callback.GetFunctionName())
        : FGUIEventDelegate());
}

UFairyButton* UPopupMenu::AddItemAt(const FString& Caption, int32 Index, FGUIEventDelegate Callback)
{
    UFairyButton* item = List->GetFromPool(List->DefaultItem)->As<UFairyButton>();
    List->AddChildAt(item, Index);

    item->SetTitle(Caption);
    item->SetGrayed(false);
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(0);
    if (Callback.IsBound())
        item->On(ClickMenu).Add(Callback);

    return item;
}

UFairyButton* UPopupMenu::AddItemAt(const FString& Caption, int32 index, const FGUIEventDynDelegate& Callback)
{
    return AddItemAt(Caption, index, Callback.IsBound() ?
        FGUIEventDelegate::CreateUFunction(const_cast<UObject*>(Callback.GetUObject()), Callback.GetFunctionName())
        : FGUIEventDelegate());
}

void UPopupMenu::AddSeperator()
{
    if (UFairyConfig::Config->PopupMenuSeperator.IsEmpty())
    {
        UE_LOG(LogFairyGUI, Warning, TEXT("UIConfig.PopupMenuSeperator not defined"));
        return;
    }

    List->AddItemFromPool(FName(UFairyConfig::Config->PopupMenuSeperator));
}

const FString& UPopupMenu::GetItemName(int32 Index) const
{
    UFairyButton* item = List->GetChildAt(Index)->As<UFairyButton>();
    return item->GetName();
}

void UPopupMenu::SetItemText(const FString& Name, const FString& Caption)
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    item->SetTitle(Caption);
}

void UPopupMenu::SetItemVisible(const FString& Name, bool bVisible)
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    if (item->IsVisible() != bVisible)
    {
        item->SetVisible(bVisible);
        List->SetBoundsChangedFlag();
    }
}

void UPopupMenu::SetItemGrayed(const FString& Name, bool bGrayed)
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    item->SetGrayed(bGrayed);
}

void UPopupMenu::SetItemCheckable(const FString& Name, bool bCheckable)
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
    {
        if (bCheckable)
        {
            if (c->GetSelectedIndex() == 0)
                c->SetSelectedIndex(1);
        }
        else
            c->SetSelectedIndex(0);
    }
}

void UPopupMenu::SetItemChecked(const FString& Name, bool bCheck)
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        c->SetSelectedIndex(bCheck ? 2 : 1);
}

bool UPopupMenu::IsItemChecked(const FString& Name) const
{
    UFairyButton* item = List->GetChild(Name)->As<UFairyButton>();
    UGController* c = item->GetController("checked");
    if (c != nullptr)
        return c->GetSelectedIndex() == 2;
    else
        return false;
}

bool UPopupMenu::RemoveItem(const FString& Name)
{
    UFairyObject* item = List->GetChild(Name);
    if (item != nullptr)
    {
        int32 index = List->GetChildIndex(item);
        List->RemoveChildToPoolAt(index);
        item->On(ClickMenu).Clear();

        return true;
    }
    else
        return false;
}

void UPopupMenu::ClearItems()
{
    int32 cnt = List->NumChildren();
    for (int32 i = 0; i < cnt; i++)
        List->GetChildAt(i)->On(ClickMenu).Clear();
    List->RemoveChildrenToPool();
}

int32 UPopupMenu::GetItemCount() const
{
    return List->NumChildren();
}

void UPopupMenu::Show(UFairyObject * AtObject, EPopupDirection Dir)
{
    UFairyApplication::Get()->GetUIRoot(this)->ShowPopup(ContentPane, AtObject, Dir);
}

void UPopupMenu::OnClickItem(UEventContext * Context)
{
    UFairyButton* item = Cast<UFairyButton>(Context->GetData().AsUObject());
    if (item == nullptr)
        return;

    if (item->IsGrayed())
    {
        List->SetSelectedIndex(-1);
        return;
    }

    UGController* c = item->GetController("checked");
    if (c != nullptr && c->GetSelectedIndex() != 0)
    {
        if (c->GetSelectedIndex() == 1)
            c->SetSelectedIndex(2);
        else
            c->SetSelectedIndex(1);
    }

    UFairyApplication::Get()->GetUIRoot(this)->HidePopup(ContentPane);

    item->DispatchEvent(ClickMenu, Context->GetData());
}

void UPopupMenu::OnAddedToStage(UEventContext * Context)
{
    List->SetSelectedIndex(-1);
    List->ResizeToFit(INT_MAX, 10);
}