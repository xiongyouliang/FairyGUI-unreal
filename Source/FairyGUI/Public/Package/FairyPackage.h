#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FairyPackage.generated.h"

class FFairyPackageItem;
class UFairyObject;
class FByteBuffer;
class UFairyPackageAsset;

UCLASS(BlueprintType)
class FAIRYGUI_API UFairyPackage : public UObject
{
    GENERATED_BODY()

public:
    static int32 Constructing;

public:
    UFairyPackage();
    virtual  ~UFairyPackage();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetID() const { return ID; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetName() const { return Name; }

    TSharedPtr<FFairyPackageItem> GetItem(const FString& ResourceID) const;
    TSharedPtr<FFairyPackageItem> GetItemByName(const FString& ResourceName);
    void* GetItemAsset(const TSharedPtr<FFairyPackageItem>& Item);

    UFairyObject* CreateObject(UObject* Owner, const FString& ResourceName);
    UFairyObject* CreateObject(UObject* Owner, const TSharedPtr<FFairyPackageItem>& Item);

private:
    void Load(FByteBuffer* Buffer);
    void LoadAtlas(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadImage(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadMovieClip(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadFont(const TSharedPtr<FFairyPackageItem>& Item);
    void LoadSound(const TSharedPtr<FFairyPackageItem>& Item);

    UObject* LoadSoundObject(FString GameAssetFilePath);

private:
    FString ID;
    FString Name;
    FString AssetPath;

    UPROPERTY(Transient)
    UFairyPackageAsset* Asset;

    TArray<TSharedPtr<FFairyPackageItem>> Items;
    TMap<FString, TSharedPtr<FFairyPackageItem>> ItemsByID;
    TMap<FString, TSharedPtr<FFairyPackageItem>> ItemsByName;
    TMap<FString, struct FAtlasSprite*> Sprites;
    
    UPROPERTY()
    TMap<FString, UObject*> SoundsByName;

    FString CustomID;
    TArray<TMap<FString, FString>> Dependencies;
    TArray<FString> Branches;
    int32 BranchIndex;

    friend class UFairyPackageMgr;
    friend class FFairyPackageItem;
    friend class UFairyApplication;
};
