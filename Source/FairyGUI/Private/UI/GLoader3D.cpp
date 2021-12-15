#include "UI/GLoader3D.h"
#include "Package/FairyPackage.h"
#include "Package/FairyPackageMgr.h"
#include "UI/FairyComponent.h"
#include "Widgets/NTexture.h"
#include "Widgets/SMovieClip.h"
#include "Widgets/SContainer.h"
#include "Utils/ByteBuffer.h"

UGLoader3D::UGLoader3D()
{
	DisplayObject = Content = SNew(SFImage).GObject(this);
}

UGLoader3D::~UGLoader3D()
{

}

void UGLoader3D::SetURL(const FString& InURL)
{
	if (URL.Compare(InURL, ESearchCase::CaseSensitive) == 0)
		return;

	ClearContent();
	URL = InURL;
	LoadContent();
	UpdateGear(7);
}

FColor UGLoader3D::NativeGetColor() const
{
	return FColor();// Content->getColor();
}

void UGLoader3D::NativeSetColor(const FColor& InColor)
{
	//Content->setColor(value);
}

void UGLoader3D::LoadContent()
{
	ClearContent();

	if (URL.IsEmpty())
		return;

	if (URL.StartsWith("ui://"))
		LoadFromPackage(URL);
	else
		LoadExternal();
}

void UGLoader3D::ClearContent()
{

}

void UGLoader3D::LoadFromPackage(const FString& ItemURL)
{
	ContentItem = UFairyPackageMgr::Get()->GetPackageItemByURL(ItemURL);

	if (ContentItem.IsValid())
	{
		ContentItem = ContentItem->GetBranch();
		//SourceSize = ContentItem->Size;
		ContentItem = ContentItem->GetHighResolution();
		ContentItem->Load();

		if (ContentItem->Type == EPackageItemType::Spine)
		{
			UpdateLayout();
		}
		else if (ContentItem->Type == EPackageItemType::DragonBones)
		{
			UpdateLayout();
		}
		else
		{
			if (bAutoSize)
				SetSize(ContentItem->Size);

			SetErrorState();

			UE_LOG(LogFairyGUI, Warning, TEXT("Unsupported type of GLoader: %d"), ContentItem->Type);
		}
	}
	else
		SetErrorState();
}

void UGLoader3D::LoadExternal()
{

}

void UGLoader3D::UpdateLayout()
{
	FVector2D SourceSize = GetSize();
	FVector2D contentSize = SourceSize;

	if (bAutoSize)
	{
		bUpdatingLayout = true;
		if (contentSize.X == 0)
			contentSize.X = 50;
		if (contentSize.Y == 0)
			contentSize.Y = 30;
		SetSize(contentSize);

		bUpdatingLayout = false;

		if (Size == contentSize)
		{
			//Content->SetPosition(FVector2D(0, 0));
			//Content->SetSize(contentSize);
			return;
		}
	}

	FVector2D ContentScale(1, 1);
	if (Fill != ELoaderFillType::None)
	{
		ContentScale = Size / SourceSize;

		if (ContentScale != FVector2D(1, 1))
		{
			if (Fill == ELoaderFillType::ScaleMatchHeight)
			{
				ContentScale.X = ContentScale.Y;
			}
			else if (Fill == ELoaderFillType::ScaleMatchWidth)
			{
				ContentScale.Y = ContentScale.X;
			}
			else if (Fill == ELoaderFillType::Scale)
			{
				if (ContentScale.X > ContentScale.Y)
				{
					ContentScale.X = ContentScale.Y;
				}
				else
				{
					ContentScale.Y = ContentScale.X;
				}
			}
			else if (Fill == ELoaderFillType::ScaleNoBorder)
			{
				if (ContentScale.X > ContentScale.Y)
				{
					ContentScale.Y = ContentScale.X;
				}
				else
				{
					ContentScale.X = ContentScale.Y;
				}
			}

			if (bShrinkOnly)
			{
				if (ContentScale.X > 1)
				{
					ContentScale.X = 1;
				}

				if (ContentScale.Y > 1)
				{
					ContentScale.Y = 1;
				}
			}

			contentSize = SourceSize * ContentScale;
		}
	}
	
	//Content->SetSize(contentSize);

	FVector2D ContentPosition;
	if (HAlign == EHAlignType::Center)
	{
		ContentPosition.X = (Size.X - contentSize.X) / 2;
	}
	else if (HAlign == EHAlignType::Right)
	{
		ContentPosition.X = Size.X - contentSize.X;
	}
	else
	{
		ContentPosition.X = 0;
	}

	if (VAlign == EVAlignType::Middle)
	{
		ContentPosition.Y = (Size.Y - contentSize.Y) / 2;
	}
	else if (VAlign == EVAlignType::Bottom)
	{
		ContentPosition.Y = Size.Y - contentSize.Y;
	}
	else
	{
		ContentPosition.Y = 0;
	}

	//Content->SetPosition(ContentPosition);
}

void UGLoader3D::SetErrorState()
{

}


FNVariant UGLoader3D::GetProp(EObjectPropID PropID) const
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		return FNVariant(GetColor());
		break;
	default:
		return UFairyObject::GetProp(PropID);
	}
}

void UGLoader3D::SetProp(EObjectPropID PropID, const FNVariant& InValue)
{
	switch (PropID)
	{
	case EObjectPropID::Color:
		SetColor(InValue.AsColor());
		break;
	default:
		UFairyObject::SetProp(PropID, InValue);
		break;
	}
}

//void UGLoader3D::HandleSizeChanged()
//{
//    UFairyObject::HandleSizeChanged();
//
//    if (!bUpdatingLayout)
//    {
//        UpdateLayout();
//    }
//}

void UGLoader3D::SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos)
{
	UFairyObject::SetupBeforeAdd(Buffer, BeginPos);

	Buffer->Seek(BeginPos, 5);
}