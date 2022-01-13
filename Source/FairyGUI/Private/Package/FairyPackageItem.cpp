#include "Package/FairyPackageItem.h"
#include "Package/FairyPackage.h"
#include "UI/FairyRoot.h"
#include "Utils/ByteBuffer.h"

FFairyPackageItem::FFairyPackageItem() :
	OwnerPackage(nullptr),
	PackageItemType(EPackageItemType::Unknown),
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

EFairyObjectType FFairyPackageItem::GetFairyObjectType()
{
	EFairyObjectType TargetObjectType = ObjectType;
	if (ObjectType == EFairyObjectType::Component && RawData.IsValid())
	{
		FairyGUI::FByteBuffer* Buffer = RawData.Get();
		int32 oldPos = Buffer->GetPos();

		Buffer->Seek(0, 0);
		Buffer->Skip(45);
		EOverflowType overflow = (EOverflowType)Buffer->ReadByte();
		if (overflow == EOverflowType::Visible)
		{
			TargetObjectType = EFairyObjectType::Component;
		}
		else if (overflow == EOverflowType::Hidden)
		{
			TargetObjectType = EFairyObjectType::ClippingComponent;
		}
		else if (overflow == EOverflowType::Scroll)
		{
			TargetObjectType = EFairyObjectType::ScrollComponent;
		}

		Buffer->SetPos(oldPos);
	}
	return TargetObjectType;
}

//void FFairyPackageItem::AddReferencedObjects(FReferenceCollector& Collector)
//{
//	if (Texture != nullptr)
//	{
//		Collector.AddReferencedObject(Texture);
//	}
//}