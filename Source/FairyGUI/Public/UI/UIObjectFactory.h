#pragma once

#include "CoreMinimal.h"
#include "FieldTypes.h"
#include "Package/FairyPackageItem.h"

class UFairyComponent;
class UGLoader;
class UFairyObject;

DECLARE_DELEGATE_RetVal(UGLoader*, FGLoaderCreator);

class FAIRYGUI_API FUIObjectFactory
{
public:
    static void SetExtension(const FString& URL, FGComponentCreator Creator);
    static void SetExtension(const FString& URL, TSubclassOf<UFairyComponent> ClassType);

    static UFairyObject* NewObject(UObject* Outer, const TSharedPtr<FFairyPackageItem>& PackageItem);
    static UFairyObject* NewObject(UObject* Outer, EFairyObjectType Type);

    static FGLoaderCreator LoaderCreator;
    static void ResolvePackageItemExtension(const TSharedPtr<FFairyPackageItem>& PackageItem);

public:
    static TMap<FString, FGComponentCreator> PackageItemExtensions;

    friend class UFGUIPackage;
};