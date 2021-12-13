#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FairyPackageMgr.generated.h"

class FFairyPackageItem;
class UFairyObject;
class FByteBuffer;
class UFairyPackageAsset;


UCLASS(BlueprintType)
class FAIRYGUI_API UFairyPackageMgr : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UFairyPackageMgr* Get();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	const FName& GetBranch() { return Branch; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetBranch(const FName& InBranch);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FName GetVar(const FName& VarKey);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVar(const FName& VarKey, const FName& VarValue);

	// Package Manager Interface start
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* AddPackage(UFairyPackageAsset* InAsset);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* GetPackageByName(const FName& PackageName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* GetPackageByID(const FName& PackageID);

	FName ConvertToItemURL(FName PackageName, FName ResourceName);
	FName NormalizeURL(FName InUrl);
	TSharedPtr<FFairyPackageItem> GetPackageItemByURL(const FString& URL);
	

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemovePackage(const FName& IDOrName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveAllPackages();
	// Package Manager Interface end

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObject(UObject* Outer, const FName& PackageName, const FName& ResourceName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI From URL", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObjectFromURL(UObject* Outer, const FName& URL);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGWindow* CreateWindow(UObject* Outer, const FName& PackageName, const FName& ResourceName);

private:
	UFairyPackageMgr();
	~UFairyPackageMgr();

	UPROPERTY()
	TMap<FName, UFairyPackage*> PackageInstByID;

	UPROPERTY()
	TMap<FName, UFairyPackage*> PackageInstByAssetPath;

	UPROPERTY()
	TMap<FName, UFairyPackage*> PackageInstByName;

	TMap<FName, FName> Vars;

	FName Branch;

	static UFairyPackageMgr* Instance;
};

