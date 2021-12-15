#include "Package/FairyPackage.h"
#include "Sound/SoundBase.h"
#include "FairyApplication.h"
#include "Package/FairyPackageAsset.h"
#include "Package/FairyPackageItem.h"
#include "UI/UIObjectFactory.h"
#include "UI/FairyObject.h"
#include "Widgets/NTexture.h"
#include "Widgets/SMovieClip.h"
#include "Widgets/BitmapFont.h"
#include "Utils/ByteBuffer.h"
#include "Package/FairyPackageMgr.h"

using namespace FairyGUI;

struct FAtlasSprite
{
    FAtlasSprite() :
        Rect(ForceInit),
        OriginalSize(ForceInit),
        Offset(ForceInit),
        bRotated(false)
    {
    }
    TSharedPtr<FFairyPackageItem> Atlas;
    FBox2D Rect;
    FVector2D OriginalSize;
    FVector2D Offset;
    bool bRotated;
};

int32 UFairyPackage::Constructing = 0;

UFairyPackage::UFairyPackage() :
    BranchIndex(0)
{
    if (HasAnyFlags(RF_ClassDefaultObject) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("UFairyPackage::UFairyPackage()"));
    }
}

UFairyPackage::~UFairyPackage()
{
    if (HasAnyFlags(RF_ClassDefaultObject) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("UFairyPackage::~UFairyPackage()"));
        for (auto& it : Sprites)
        {
            delete it.Value;
        }
    }
}

TSharedPtr<FFairyPackageItem> UFairyPackage::GetItem(const FName& ItemID) const
{
    auto it = ItemsByID.Find(ItemID);
    if (it != nullptr)
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

TSharedPtr<FFairyPackageItem> UFairyPackage::GetItemByName(const FName& ResourceName)
{
    auto it = ItemsByName.Find(ResourceName);
    if (it != nullptr)
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

UFairyObject* UFairyPackage::CreateObject(UObject* Owner, const FName& ComponentName)
{
    TSharedPtr<FFairyPackageItem> item = GetItemByName(ComponentName);
    //verifyf will break app, use a error log replace it in Dev/Test/Debug Build, include Editor;
#if UE_BUILD_SHIPPING
    verifyf(item.IsValid(), TEXT("FairyGUI: resource not found - %s in  %s"), *ResourceName, *Name);
#else
    if (!item.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("FairyGUI: resource not found - %s in  %s"), *ComponentName.ToString(), *Name.ToString());
        return nullptr;
    }
#endif // UE_BUILD_SHIPPING
    return CreateObject(Owner, item);
}

UFairyObject* UFairyPackage::CreateObject(UObject* Owner, const TSharedPtr<FFairyPackageItem>& Item)
{
    UFairyObject* Object = FUIObjectFactory::NewObject(Owner, Item);
    if (Object == nullptr) 
    {
        return nullptr;
    }

    Constructing++;
    Object->ConstructFromResource();
    Constructing--;
    return Object;
}

void UFairyPackage::Load(FairyGUI::FByteBuffer* Buffer)
{
    // Is the first 4 bytes is check code?
    uint32 checkCode = Buffer->ReadUint();
    VersionCode = Buffer->ReadUint();
    if ( checkCode != 0x46475549)
    {
        UE_LOG(LogFairyGUI, Error, TEXT("not valid package format in %d '%s'"), VersionCode, *AssetPath.ToString());
        return;
    }
    Buffer->Version = VersionCode; // todo: Version property has add in UFairyPackage, this will remove in next refactor;

    bool ver2 = VersionCode >= 2;

    bool IsCompressed = Buffer->ReadBool(); //compressed

    ID = FName(Buffer->ReadString());
    Name = FName(Buffer->ReadString());

    Buffer->Skip(20);

    int32 indexTablePos = Buffer->GetPos();
    
    // string table segment: index = 4
    Buffer->Seek(indexTablePos, 4);
    int32 strNum = Buffer->ReadInt();
    TArray<FString>* StringTable = new TArray<FString>();
    StringTable->SetNum(strNum, true);
    for (int32 i = 0; i < strNum; i++)
    {
        (*StringTable)[i] = Buffer->ReadString();
    }
    Buffer->StringTable = MakeShareable(StringTable);

    // dependency and branch segment: index = 0
    Buffer->Seek(indexTablePos, 0);
    int32 depNum = Buffer->ReadShort();
    for (int32 i = 0; i < depNum; i++)
    {
        TMap<FName, FName> info;
        info.Add(FName(TEXT("id")), Buffer->ReadFNameFromCache());
        info.Add(FName(TEXT("name")), Buffer->ReadFNameFromCache());

        Dependencies.Push(info);
    }

    bool branchIncluded = false;
    if (ver2)
    {
        int branchNum = Buffer->ReadShort();
        if (branchNum > 0)
        {
            Buffer->ReadFNameArray(Branches, branchNum);
            FName CurBranch = UFairyPackageMgr::Get()->GetBranch();
            if (!CurBranch.IsNone())
            {
                BranchIndex = Branches.IndexOfByKey(CurBranch);
            }
        }

        branchIncluded = branchNum > 0;
    }

    // all item info segument in this package: index = 1;
    Buffer->Seek(indexTablePos, 1);
    FString path = FPaths::GetPath(AssetPath.ToString());
    FString fileName = FPaths::GetBaseFilename(AssetPath.ToString());

    int32 itemNum = Buffer->ReadShort();
    for (int32 i = 0; i < itemNum; i++)
    {
        int32 nextPos = Buffer->ReadInt();
        nextPos += Buffer->GetPos();

        TSharedPtr<FFairyPackageItem> pi = MakeShared<FFairyPackageItem>();
        pi->OwnerPackage = this;
        pi->Type = (EPackageItemType)Buffer->ReadByte();
        pi->ID = Buffer->ReadFNameFromCache();
        pi->Name = Buffer->ReadFNameFromCache();
        Buffer->Skip(2); //path
        pi->File = Buffer->ReadFNameFromCache();
        Buffer->ReadBool(); //exported
        pi->Size.X = Buffer->ReadInt();
        pi->Size.Y = Buffer->ReadInt();

        switch (pi->Type)
        {
        case EPackageItemType::Image:
        {
            pi->ObjectType = EObjectType::Image;
            int32 scaleOption = Buffer->ReadByte();
            if (scaleOption == 1)
            {
                FBox2D scale9Grid(ForceInit);
                scale9Grid.Min.X = Buffer->ReadInt();
                scale9Grid.Min.Y = Buffer->ReadInt();
                scale9Grid.Max.X = scale9Grid.Min.X + Buffer->ReadInt();
                scale9Grid.Max.Y = scale9Grid.Min.Y + Buffer->ReadInt();
                pi->Scale9Grid = scale9Grid;
                pi->TileGridIndice = Buffer->ReadInt();
            }
            else if (scaleOption == 2)
            {
                pi->bScaleByTile = true;
            }

            Buffer->ReadBool(); //smoothing
            break;
        }

        case EPackageItemType::MovieClip:
        {
            Buffer->ReadBool(); //smoothing
            pi->ObjectType = EObjectType::MovieClip;
            pi->RawData = Buffer->ReadBuffer(false);
            break;
        }

        case EPackageItemType::Font:
        {
            pi->RawData = Buffer->ReadBuffer(false);
            break;
        }

        case EPackageItemType::Component:
        {
            int32 extension = Buffer->ReadByte();
            if (extension > 0)
            {
                pi->ObjectType = (EObjectType)extension;
            }
            else
            {
                pi->ObjectType = EObjectType::Component;
            }
            pi->RawData = Buffer->ReadBuffer(false);

            FUIObjectFactory::ResolvePackageItemExtension(pi);
            break;
        }

        case EPackageItemType::Atlas:
        case EPackageItemType::Sound:
        case EPackageItemType::Misc:
        {
            FString file = fileName + "_" + FPaths::GetBaseFilename(pi->File.ToString());
            pi->File = FName(path + "/" + file + "." + file);
            break;
        }

        case EPackageItemType::Spine:
        case EPackageItemType::DragonBones:
        {
            pi->File = FName(path + pi->File.ToString());
            break;
        }

        default:
            break;
        }

        if (ver2)
        {
            FName BranchStr = Buffer->ReadFNameFromCache(); //branch
            if (!BranchStr.IsNone())
            {
                // construct a new FName;
                pi->Name = FName(BranchStr.ToString() + "/" + pi->Name.ToString());
            }

            int32 branchCnt = Buffer->ReadUbyte();
            if (branchCnt > 0)
            {
                if (branchIncluded)
                {
                    pi->Branches.Emplace();
                    Buffer->ReadFNameArray(pi->Branches.GetValue(), branchCnt);
                }
                else
                {
                    ItemsByID.Add(Buffer->ReadFNameFromCache(), pi);
                }
            }

            int32 highResCnt = Buffer->ReadUbyte();
            if (highResCnt > 0)
            {
                pi->HighResolution.Emplace();
                Buffer->ReadFNameArray(pi->HighResolution.GetValue(), highResCnt);
            }
        }

        Items.Push(pi);
        ItemsByID.Add(pi->ID, pi);
        if (!pi->Name.IsNone())
        {
            ItemsByName.Add(pi->Name, pi);
        }

        Buffer->SetPos(nextPos);
    }

    // Atlas Sprite info segment: index = 2
    Buffer->Seek(indexTablePos, 2);
    int32 atlasNum = Buffer->ReadShort();
    for (int32 i = 0; i < atlasNum; i++)
    {
        int32 nextPos = Buffer->ReadShort();
        nextPos += Buffer->GetPos();

        FName itemId = Buffer->ReadFNameFromCache();
        FName atlasItemId = Buffer->ReadFNameFromCache();
        const TSharedPtr<FFairyPackageItem>& atlasPackageItem = ItemsByID[atlasItemId];

        FAtlasSprite* sprite = new FAtlasSprite();
        sprite->Atlas = atlasPackageItem;
        sprite->Rect.Min.X = Buffer->ReadInt();
        sprite->Rect.Min.Y = Buffer->ReadInt();
        sprite->Rect.Max.X = sprite->Rect.Min.X + Buffer->ReadInt();
        sprite->Rect.Max.Y = sprite->Rect.Min.Y + Buffer->ReadInt();
        sprite->bRotated = Buffer->ReadBool();
        if (ver2 && Buffer->ReadBool())
        {
            sprite->Offset.X = Buffer->ReadInt();
            sprite->Offset.Y = Buffer->ReadInt();
            sprite->OriginalSize.X = Buffer->ReadInt();
            sprite->OriginalSize.Y = Buffer->ReadInt();
        }
        else if (sprite->bRotated)
        {
            sprite->Offset.Set(0, 0);
            sprite->OriginalSize.X = sprite->Rect.GetSize().Y;
            sprite->OriginalSize.Y = sprite->Rect.GetSize().X;
        }
        else
        {
            sprite->Offset.Set(0, 0);
            sprite->OriginalSize = sprite->Rect.GetSize();
        }
        Sprites.Add(itemId, sprite);

        Buffer->SetPos(nextPos);
    }

    //if (Buffer->seek(indexTablePos, 3))
    //{
    //    cnt = buffer->readShort();
    //    for (int32 i = 0; i < cnt; i++)
    //    {
    //        int32 nextPos = buffer->readInt();
    //        nextPos += buffer->getPos();
    //        auto it = ItemsByID.find(buffer->readS());
    //        if (it != ItemsByID.end())
    //        {
    //            pi = it->second;
    //            if (pi->type == PackageItemType::IMAGE)
    //            {
    //                pi->pixelHitTestData = new PixelHitTestData();
    //                pi->pixelHitTestData->load(buffer);
    //            }
    //        }
    //        buffer->setPos(nextPos);
    //    }
    //}

}

void* UFairyPackage::GetItemAsset(const TSharedPtr<FFairyPackageItem>& Item)
{
    switch (Item->Type)
    {
    case EPackageItemType::Image:
        if (Item->Texture == nullptr)
        {
            LoadImage(Item);
        }
        return Item->Texture;

    case EPackageItemType::Atlas:
        if (Item->Texture == nullptr)
        {
            LoadAtlas(Item);
        }
        return Item->Texture;

    case EPackageItemType::Font:
        if (Item->BitmapFont == nullptr)
        {
            LoadFont(Item);
        }
        return Item->BitmapFont.Get();

    case EPackageItemType::MovieClip:
        if (!Item->MovieClipData.IsValid())
        {
            LoadMovieClip(Item);
        }
        return Item->MovieClipData.Get();

    case EPackageItemType::Sound:
        LoadSound(Item);
        return &(Item->Sound);

    default:
        return nullptr;
    }
}

void UFairyPackage::LoadAtlas(const TSharedPtr<FFairyPackageItem>& Item)
{
    UObject* Texture = StaticLoadObject(UTexture2D::StaticClass(), this, *Item->File.ToString());
    Item->Texture = NewObject<UNTexture>(this);
    Item->Texture->Init(Cast<UTexture2D>(Texture));
}

void UFairyPackage::LoadImage(const TSharedPtr<FFairyPackageItem>& Item)
{
    FAtlasSprite* sprite = Sprites.FindRef(FName(Item->ID));
    if (sprite != nullptr)
    {
        UNTexture* atlas = (UNTexture*)GetItemAsset(sprite->Atlas);
        if (atlas->GetSize() == sprite->Rect.GetSize())
        {
            Item->Texture = atlas;
        }
        else
        {
            Item->Texture = NewObject<UNTexture>(this);
            Item->Texture->Init(atlas, sprite->Rect, sprite->bRotated, sprite->OriginalSize, sprite->Offset);
        }
    }
}

void UFairyPackage::LoadMovieClip(const TSharedPtr<FFairyPackageItem>& Item)
{
    TSharedPtr<FMovieClipData> Data = MakeShared<FMovieClipData>();
    Item->MovieClipData = Data;
    FairyGUI::FByteBuffer* Buffer = Item->RawData.Get();

    Buffer->Seek(0, 0);

    Data->Interval = Buffer->ReadInt() / 1000.0f;
    Data->bSwing = Buffer->ReadBool();
    Data->RepeatDelay = Buffer->ReadInt() / 1000.0f;

    Buffer->Seek(0, 1);

    int32 frameCount = Buffer->ReadShort();
    Data->Frames.Reserve(frameCount);

    FAtlasSprite* sprite;

    for (int32 i = 0; i < frameCount; i++)
    {
        int32 nextPos = Buffer->ReadShort();
        nextPos += Buffer->GetPos();

        FMovieClipData::Frame Frame;

        FBox2D FrameRect;
        FrameRect.Min.X = Buffer->ReadInt();
        FrameRect.Min.Y = Buffer->ReadInt();
        FrameRect.Max.X = FrameRect.Min.X + Buffer->ReadInt();
        FrameRect.Max.Y = FrameRect.Min.Y + Buffer->ReadInt();
        Frame.AddDelay = Buffer->ReadInt() / 1000.0f;
        const FString& spriteId = Buffer->ReadStringFromCache();

        if (!spriteId.IsEmpty() && (sprite = Sprites.FindRef(FName(spriteId))) != nullptr)
        {
            Frame.Texture = NewObject<UNTexture>(this);
            Frame.Texture->Init((UNTexture*)GetItemAsset(sprite->Atlas), sprite->Rect, sprite->bRotated, Item->Size, FrameRect.Min);
        }

        Data->Frames.Add(MoveTemp(Frame));

        Buffer->SetPos(nextPos);
    }

    Item->RawData.Reset();
}

void UFairyPackage::LoadFont(const TSharedPtr<FFairyPackageItem>& Item)
{
    TSharedPtr<FBitmapFont> BitmapFont = MakeShared<FBitmapFont>();
    Item->BitmapFont = BitmapFont;
    FairyGUI::FByteBuffer* Buffer = Item->RawData.Get();

    Buffer->Seek(0, 0);

    bool bTTF = Buffer->ReadBool();
    BitmapFont->bCanTint = Buffer->ReadBool();
    BitmapFont->bResizable = Buffer->ReadBool();
    BitmapFont->bHasChannel = Buffer->ReadBool(); //hasChannel
    BitmapFont->FontSize = Buffer->ReadInt();
    int32 XAdvance = Buffer->ReadInt();
    int32 LineHeight = Buffer->ReadInt();

    FVector2D GlyphTexCoords(0, 0);
    FVector2D GlyphOffset(0, 0);
    FVector2D GlyphSize(0, 0);

    const FAtlasSprite* MainSprite = nullptr;
    if (bTTF && (MainSprite = Sprites.FindRef(FName(Item->ID))) != nullptr)
    {
        BitmapFont->Texture = (UNTexture*)GetItemAsset(MainSprite->Atlas);
    }

    Buffer->Seek(0, 1);

    int32 cnt = Buffer->ReadInt();
    for (int32 i = 0; i < cnt; i++)
    {
        int32 nextPos = Buffer->ReadShort();
        nextPos += Buffer->GetPos();

        FBitmapFont::FGlyph Glyph;

        TCHAR ch = Buffer->ReadUshort();
        const FString& img = Buffer->ReadStringFromCache();
        GlyphTexCoords.X = Buffer->ReadInt();
        GlyphTexCoords.Y = Buffer->ReadInt();
        GlyphOffset.X = Buffer->ReadInt();
        GlyphOffset.Y = Buffer->ReadInt();
        GlyphSize.X = Buffer->ReadInt();
        GlyphSize.Y = Buffer->ReadInt();
        Glyph.XAdvance = Buffer->ReadInt();
        Glyph.Channel = Buffer->ReadByte();

        if (bTTF)
        {
            FVector2D TexCoords = MainSprite->Rect.Min + GlyphTexCoords;
            Glyph.UVRect = FBox2D(TexCoords / BitmapFont->Texture->GetSize(), (TexCoords + GlyphSize) / BitmapFont->Texture->GetSize());
            Glyph.Offset = GlyphOffset;
            Glyph.Size = GlyphSize;
            Glyph.LineHeight = LineHeight;
        }
        else
        {
            TSharedPtr<FFairyPackageItem> CharImg = GetItem(FName(img));
            if (CharImg.IsValid())
            {
                CharImg = CharImg->GetBranch();
                GlyphSize = CharImg->Size;
                CharImg = CharImg->GetHighResolution();
                GetItemAsset(CharImg);
                Glyph.UVRect = CharImg->Texture->UVRect;

                FVector2D TexScale = GlyphSize / CharImg->Size;

                Glyph.Offset = GlyphOffset + CharImg->Texture->Offset * TexScale;
                Glyph.Size = CharImg->Size * TexScale;

                if (BitmapFont->Texture == nullptr)
                {
                    BitmapFont->Texture = CharImg->Texture->Root;
                }
            }

            if (BitmapFont->FontSize == 0)
            {
                BitmapFont->FontSize = (int32)GlyphSize.Y;
            }

            if (Glyph.XAdvance == 0)
            {
                if (XAdvance == 0)
                {
                    Glyph.XAdvance = GlyphOffset.X + GlyphSize.X;
                }
                else
                {
                    Glyph.XAdvance = XAdvance;
                }
            }

            Glyph.LineHeight = GlyphOffset.Y < 0 ? GlyphSize.Y : (GlyphOffset.Y + GlyphSize.Y);
            if (Glyph.LineHeight < BitmapFont->FontSize)
            {
                Glyph.LineHeight = BitmapFont->FontSize;
            }
        }

        BitmapFont->Glyphs.Add(ch, Glyph);
        Buffer->SetPos(nextPos);
    }

    Item->RawData.Reset();
}

void UFairyPackage::LoadSound(const TSharedPtr<FFairyPackageItem>& Item)
{
    FSlateSound Sound;
    UObject* SoundBase = SoundsByName.Num() > 0 ? *SoundsByName.Find(Item->File) : nullptr;
    if (SoundBase == nullptr)
    {
        SoundBase = LoadSoundObject(Item->File);
        SoundsByName.Add(Item->File, SoundBase);
    }
    Sound.SetResourceObject(SoundBase);
    Item->Sound = Sound;
}

UObject* UFairyPackage::LoadSoundObject(FName GameAssetFilePath)
{
    UObject* SoundBaseObject = nullptr;
	UObject* ResourceObject = StaticFindObject(UObject::StaticClass(), this, *GameAssetFilePath.ToString());
	if (!ResourceObject)
	{
		 SoundBaseObject = StaticLoadObject(USoundBase::StaticClass(), this, *GameAssetFilePath.ToString());
	}
    return SoundBaseObject;
}