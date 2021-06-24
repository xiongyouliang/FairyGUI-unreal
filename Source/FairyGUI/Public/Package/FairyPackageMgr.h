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
	const FString& GetBranch() { return Branch; }

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetBranch(const FString& InBranch);


	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	FString GetVar(const FString& VarKey);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void SetVar(const FString& VarKey, const FString& VarValue);

	// Package Manager Interface start
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* AddPackage(UFairyPackageAsset* InAsset);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* GetPackageByName(const FString& PackageName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UFairyPackage* GetPackageByID(const FString& PackageID);

	FString ConvertToItemURL(const FString& PackageName, const FString& ResourceName);
	FString NormalizeURL(const FString& URL);
	TSharedPtr<FFairyPackageItem> GetPackageItemByURL(const FString& URL);
	

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemovePackage(const FString& IDOrName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	void RemoveAllPackages();
	// Package Manager Interface end

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObject(UObject* Outer, const FString& PackageName, const FString& ResourceName);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (DisplayName = "Create UI From URL", DeterminesOutputType = "ClassType"))
	UFairyObject* CreateObjectFromURL(UObject* Outer, const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	UGWindow* CreateWindow(UObject* Outer, const FString& PackageName, const FString& ResourceName);

private:
	UFairyPackageMgr();
	~UFairyPackageMgr();

	UPROPERTY()
	TMap<FString, UFairyPackage*> PackageInstByID;
	UPROPERTY()
	TMap<FString, UFairyPackage*> PackageInstByName;
	TMap<FString, FString> Vars;
	FString Branch;

	static UFairyPackageMgr* Instance;
};

