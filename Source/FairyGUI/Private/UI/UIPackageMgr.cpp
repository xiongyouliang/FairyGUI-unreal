#include "UI/UIPackageMgr.h"

#include "FairyApplication.h"
#include "UI/UIPackage.h"
#include "FairyCommons.h"

TMap<FString, UUIPackage*> UUIPackageMgr::PackageInstByID;
TMap<FString, UUIPackage*> UUIPackageMgr::PackageInstByName;
TMap<FString, FString> UUIPackageMgr::Vars;
FString UUIPackageMgr::Branch;

void UUIPackageMgr::SetBranch(const FString& InBranch)
{
	Branch = InBranch;
	bool empty = Branch.IsEmpty();
	for (auto& it : PackageInstByID)
	{
		UUIPackage*& Pkg = it.Value;
		if (empty)
			Pkg->BranchIndex = -1;
		else if (Pkg->Branches.Num() > 0)
			Pkg->BranchIndex = Pkg->Branches.IndexOfByKey(Branch);
	}
}

FString UUIPackageMgr::GetVar(const FString& VarKey)
{
	FString* Value = Vars.Find(VarKey);
	if (Value != nullptr)
		return *Value;
	else
		return G_EMPTY_STRING;
}

void UUIPackageMgr::SetVar(const FString& VarKey, const FString& VarValue)
{
	Vars.Add(VarKey, VarValue);
}

UUIPackage* UUIPackageMgr::AddPackage(UObject* Outer, UUIPackageAsset* InAsset)
{
	UUIPackage* Pkg = PackageInstByID.FindRef(InAsset->GetPathName());
	if (Pkg != nullptr)
	{
		UE_LOG(LogFairyGUI, Warning, TEXT("Package already addedd"));
		return Pkg;
	}
	Pkg = NewObject<UUIPackage>(Outer);
	Pkg->Asset = InAsset;
	Pkg->AssetPath = InAsset->GetPathName();
	FByteBuffer Buffer(InAsset->Data.GetData(), 0, InAsset->Data.Num(), false);
	Pkg->Load(&Buffer);

	//UFairyApplication::Get()->PackageList.Add(Pkg);
	PackageInstByID.Add(Pkg->ID, Pkg);
	PackageInstByID.Add(Pkg->AssetPath, Pkg);
	PackageInstByName.Add(Pkg->Name, Pkg);
	return Pkg;
}

UUIPackage* UUIPackageMgr::GetPackageByName(const FString& PackageName)
{
	auto it = UUIPackageMgr::PackageInstByName.Find(PackageName);
	if (it != nullptr)
		return *it;
	else
		return nullptr;
}

UUIPackage* UUIPackageMgr::GetPackageByID(const FString& PackageID)
{
	auto it = UUIPackageMgr::PackageInstByID.Find(PackageID);
	if (it != nullptr)
		return *it;
	else
		return nullptr;
}

FString UUIPackageMgr::ConvertToItemURL(const FString& PackageName, const FString& ResourceName)
{
	UUIPackage* pkg = UUIPackageMgr::GetPackageByName(PackageName);
	if (pkg != nullptr)
	{
		TSharedPtr<FPackageItem> pi = pkg->GetItemByName(ResourceName);
		if (pi.IsValid())
			return "ui://" + pkg->GetID() + pi->ID;
	}
	return "";
}

FString UUIPackageMgr::NormalizeURL(const FString& URL)
{
	if (URL.IsEmpty()) 
	{
		return URL;
	}

	int32 pos1;
	if (!URL.FindChar('/', pos1)) 
	{
		return URL;
	}

	int32 pos2 = URL.Find("/", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1 + 2);
	if (pos2 == -1) 
	{
		return URL;
	}	
	else
	{
		FString pkgName = URL.Mid(pos1 + 2, pos2 - pos1 - 2);
		FString srcName = URL.Mid(pos2 + 1);
		return ConvertToItemURL(pkgName, srcName);
	}
}
TSharedPtr<FPackageItem> UUIPackageMgr::GetPackageItemByURL(const FString& URL)
{
	if (URL.IsEmpty()) 
	{
		return nullptr;
	}
		
	int32 pos1;
	if (!URL.FindChar('/', pos1)) {
		return nullptr;
	}

	int32 pos2 = URL.Find("/", ESearchCase::CaseSensitive, ESearchDir::FromStart, pos1 + 2);
	if (pos2 == -1) {
		if (URL.Len() > 13)
		{
			FString pkgId = URL.Mid(5, 8);
			UUIPackage* pkg = UUIPackageMgr::GetPackageByID(pkgId);
			if (pkg != nullptr)
			{
				FString srcId = URL.Mid(13);
				return pkg->GetItem(srcId);
			}
		}
	}
	else
	{
		FString pkgName = URL.Mid(pos1 + 2, pos2 - pos1 - 2);
		UUIPackage* pkg = UUIPackageMgr::GetPackageByName(pkgName);
		if (pkg != nullptr)
		{
			FString srcName = URL.Mid(pos2 + 1);
			return pkg->GetItemByName(srcName);
		}
	}

	return nullptr;
}

void UUIPackageMgr::RemovePackage(const FString& IDOrName)
{
	UUIPackage* pkg = GetPackageByName(IDOrName);
	if (pkg == nullptr)
	{
		pkg = UUIPackageMgr::GetPackageByID(IDOrName);
	}

	if (pkg != nullptr)
	{
		//TArray<UUIPackage*>& PackageList = UFairyApplication::Get()->PackageList;
		//PackageList.Remove(pkg);

		PackageInstByID.Remove(pkg->ID);
		PackageInstByName.Remove(pkg->Name);
	}
	else
		UE_LOG(LogFairyGUI, Error, TEXT("invalid package name or id: %s"), *IDOrName);
}

void UUIPackageMgr::RemoveAllPackages()
{
	/*TArray<UUIPackage*>& PackageList = UFairyApplication::Get()->PackageList;
	PackageList.Reset();*/

	PackageInstByID.Reset();
	PackageInstByName.Reset();
}

UGObject* UUIPackageMgr::CreateObject(UObject* Outer, const FString& PackageName, const FString& ResourceName)
{
	UUIPackage* pkg = UUIPackageMgr::GetPackageByName(PackageName);
	if (pkg) {
		return pkg->CreateObject(Outer, ResourceName);
	}
	else
	{
		return nullptr;
	}
}

UGObject* UUIPackageMgr::CreateObjectFromURL(UObject* Outer, const FString& URL)
{
	TSharedPtr<FPackageItem> PackageItem = UUIPackageMgr::GetPackageItemByURL(URL);
	if (PackageItem.IsValid()) {
		return PackageItem->Owner->CreateObject(Outer, PackageItem);
	}
	else
	{
		return nullptr;
	}
}

UGWindow* UUIPackageMgr::CreateWindow(UObject* Outer, const FString& PackageName, const FString& ResourceName)
{
	UGObject* ContentPane = CreateObject(Outer, PackageName, ResourceName);
	verifyf(ContentPane->IsA<UGComponent>(), TEXT("Window content should be a component"));
	UGWindow* Window = NewObject<UGWindow>(Outer);
	Window->SetContentPane(Cast<UGComponent>(ContentPane));

	return Window;
}