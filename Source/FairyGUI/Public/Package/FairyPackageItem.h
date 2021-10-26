#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UI/FieldTypes.h"
#include "FairyCommons.h"

class FByteBuffer;
struct FMovieClipData;
struct FBitmapFont;

class UFairyPackage;
class UNTexture;
class UFairyComponent;

class FAIRYGUI_API FFairyPackageItem : public FGCObject, public TSharedFromThis<FFairyPackageItem>
{
public:
    FFairyPackageItem();

    void Load();
    TSharedPtr<FFairyPackageItem> GetBranch();
    TSharedPtr<FFairyPackageItem> GetHighResolution();

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

public:
    UFairyPackage* OwnerPackage;

    EPackageItemType Type;
    EObjectType ObjectType;
    FString ID;
    FString Name;
    FVector2D Size;
    FString File;
    TSharedPtr<FByteBuffer> RawData;
    TOptional<TArray<FString>> Branches;
    TOptional<TArray<FString>> HighResolution;

    //atlas
    UNTexture* Texture;

    //image
    TOptional<FBox2D> Scale9Grid;
    bool bScaleByTile;
    int32 TileGridIndice;
    //PixelHitTestData* pixelHitTestData;

    //movie clip
    TSharedPtr<FMovieClipData> MovieClipData;

    //component
    FGComponentCreator ExtensionCreator;
    bool bTranslated;

    //font
    TSharedPtr<FBitmapFont> BitmapFont;

    //sound
    FSlateSound Sound;
};
