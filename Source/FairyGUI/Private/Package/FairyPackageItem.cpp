#include "Package/FairyPackageItem.h"
#include "Package/FairyPackage.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

FFairyPackageItem::FFairyPackageItem() :
    OwnerPackage(nullptr),
    Type(EPackageItemType::Unknown),
    ObjectType(EObjectType::Component),
    Size(0, 0),
    Texture(nullptr),
    bScaleByTile(false),
    TileGridIndice(0)
{
}

void FFairyPackageItem::Load()
{
    if (OwnerPackage)
    {
        OwnerPackage->GetItemAsset(AsShared());
    }
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetBranch()
{
    if (Branches.IsSet() && OwnerPackage->BranchIndex != -1)
    {
        const FName& ItemID = Branches.GetValue()[OwnerPackage->BranchIndex];
        if (!ItemID.IsNone())
        {
			return OwnerPackage->GetItem(ItemID);
        }
    }

    return AsShared();
}

TSharedPtr<FFairyPackageItem> FFairyPackageItem::GetHighResolution()
{
    if (HighResolution.IsSet() && UFairyRoot::ContentScaleLevel > 0)
    {
        FName ItemID = HighResolution.GetValue()[UFairyRoot::ContentScaleLevel - 1];
        if (!ItemID.IsNone())
        {
			return OwnerPackage->GetItem(ItemID);
        }
    }

    return AsShared();
}

void FFairyPackageItem::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (Texture != nullptr)
    {
		Collector.AddReferencedObject(Texture);
    }
}