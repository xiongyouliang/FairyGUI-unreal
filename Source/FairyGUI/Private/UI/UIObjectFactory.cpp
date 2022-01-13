#include "UI/UIObjectFactory.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageItem.h"
#include "UI/FairyComponent.h"
#include "UI/GImage.h"
#include "UI/FairyMovieClip.h"
#include "UI/GTextField.h"
#include "UI/GRichTextField.h"
#include "UI/GTextInput.h"
#include "UI/GGraph.h"
#include "UI/GLoader.h"
#include "UI/GLoader3D.h"
#include "UI/GGroup.h"
#include "UI/GLabel.h"
#include "UI/FairyButton.h"
#include "UI/GComboBox.h"
#include "UI/GProgressBar.h"
#include "UI/GSlider.h"
#include "UI/GScrollBar.h"
#include "UI/FairyListView.h"
#include "UI/GTree.h"
#include "Package/FairyPackageMgr.h"

TMap<FString, FGComponentCreator> FUIObjectFactory::PackageItemExtensions;
FGLoaderCreator FUIObjectFactory::LoaderCreator;

void FUIObjectFactory::SetExtension(const FString& URL, FGComponentCreator Creator)
{
    if (URL.IsEmpty())
    {
        UE_LOG(LogFairyGUI, Warning, TEXT("Invaild url: %s"), *URL);
        return;
    }
    TSharedPtr<FFairyPackageItem> PackageItem = UFairyPackageMgr::Get()->GetPackageItemByURL(URL);
    if (PackageItem.IsValid())
    {
        PackageItem->ExtensionCreator = Creator;
    }

    PackageItemExtensions.Add(URL, Creator);
}

void FUIObjectFactory::SetExtension(const FString& URL, TSubclassOf<UFairyComponent> ClassType)
{
    SetExtension(URL, FGComponentCreator::CreateLambda([ClassType]() {
        return ::NewObject<UFairyComponent>(GetTransientPackage(), ClassType);
    }));
}

UFairyObject* FUIObjectFactory::NewObject(UObject* Outer, const TSharedPtr<FFairyPackageItem>& PackageItem)
{
    UFairyObject* FairyObject = nullptr;
    if (PackageItem->ExtensionCreator.IsBound())
    {
        FairyObject = PackageItem->ExtensionCreator.Execute();
    }
    else
    {
        FairyObject = FUIObjectFactory::NewObject(Outer, PackageItem->GetFairyObjectType());
    }

    if (FairyObject != nullptr)
    {
        FairyObject->SetPackageItem(PackageItem);
    }
    return FairyObject;
}

UFairyObject* FUIObjectFactory::NewObject(UObject* Outer, EFairyObjectType Type)
{
    switch (Type)
    {
    case EFairyObjectType::Image:
        return ::NewObject<UGImage>(Outer);
    case EFairyObjectType::MovieClip:
        return ::NewObject<UFairyMovieClip>(Outer);
    case EFairyObjectType::Component:
        return ::NewObject<UFairyComponent>(Outer);
	case EFairyObjectType::ClippingComponent:
		return ::NewObject<UFairyComponent>(Outer);
	case EFairyObjectType::ScrollComponent:
		return ::NewObject<UFairyComponent>(Outer);
    case EFairyObjectType::Text:
        return ::NewObject<UGTextField>(Outer);
    case EFairyObjectType::RichText:
        return ::NewObject<UGRichTextField>(Outer);
    case EFairyObjectType::InputText:
        return ::NewObject<UGTextInput>(Outer);
    case EFairyObjectType::Group:
        return ::NewObject<UGGroup>(Outer);
    case EFairyObjectType::List:
        return ::NewObject<UFairyListView>(Outer);
    case EFairyObjectType::Graph:
        return ::NewObject<UGGraph>(Outer);
    case EFairyObjectType::Loader:
        if (LoaderCreator.IsBound())
        {
            return LoaderCreator.Execute();
        }
        else
        {
            return ::NewObject<UGLoader>(Outer);
        }
    case EFairyObjectType::Button:
        return ::NewObject<UFairyButton>(Outer);
    case EFairyObjectType::Label:
        return ::NewObject<UGLabel>(Outer);
    case EFairyObjectType::ProgressBar:
        return ::NewObject<UGProgressBar>(Outer);
    case EFairyObjectType::Slider:
        return ::NewObject<UGSlider>(Outer);
    case EFairyObjectType::ScrollBar:
        return ::NewObject<UGScrollBar>(Outer);
    case EFairyObjectType::ComboBox:
        return ::NewObject<UGComboBox>(Outer);
    case EFairyObjectType::Tree:
        return ::NewObject<UGTree>(Outer);
    case EFairyObjectType::Loader3D:
        return ::NewObject<UGLoader3D>(Outer);
    default:
        return nullptr;
    }
}

void FUIObjectFactory::ResolvePackageItemExtension(const TSharedPtr<FFairyPackageItem>& PackageItem)
{
    auto it = PackageItemExtensions.Find(FString(TEXT("ui://") + PackageItem->GetOwnerPackage()->GetID().ToString() + PackageItem->ID.ToString()));
    if (it != nullptr)
    {
        PackageItem->ExtensionCreator = *it;
        return;
    }
    it = PackageItemExtensions.Find(FString(TEXT("ui://") + PackageItem->GetOwnerPackage()->GetName().ToString() + TEXT("/") + PackageItem->Name.ToString()));
    if (it != nullptr)
    {
        PackageItem->ExtensionCreator = *it;
        return;
    }
    PackageItem->ExtensionCreator = nullptr;
}
