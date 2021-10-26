#include "Widgets/NGraphics.h"

FNGraphics::FNGraphics() :
    Color(FColor::White),
    Flip(EFlipType::None),
    Texture(nullptr),
    UsingAlpha(1),
    bMeshDirty(false)
{
}

FNGraphics::~FNGraphics()
{
}

void FNGraphics::SetColor(const FColor& InColor)
{
    if (Color != InColor)
    {
        bMeshDirty = true;
        Color = InColor;
    }
}

void FNGraphics::SetFlip(EFlipType InFlip)
{
    if (Flip != InFlip)
    {
        Flip = InFlip;
        bMeshDirty = true;
    }
}

void FNGraphics::SetMeshFactory(const TSharedPtr<IMeshFactory>& InMeshFactory)
{
    MeshFactory = InMeshFactory;
    bMeshDirty = true;
}

void FNGraphics::SetTexture(UNTexture* InTexture)
{
    if (InTexture != Texture)
    {
        Texture = InTexture;
        if (InTexture != nullptr)
        {
            Brush.SetResourceObject(InTexture->NativeTexture);
            Brush.SetImageSize(InTexture->GetSize());
            //static const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");
            ResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(Brush);
        }
        else
        {
            Brush.SetResourceObject(nullptr);
            ResourceHandle = FSlateResourceHandle();
        }
        bMeshDirty = true;
    }
}

void FNGraphics::Paint(const FGeometry& AllottedGeometry,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    float InAlpha,
    bool bEnabled)
{
    UpdateMeshNow(AllottedGeometry, InAlpha);
    const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, ResourceHandle, Vertices, Triangles, nullptr, 0, 0, DrawEffects);
}

void FNGraphics::UpdateMeshNow(const FGeometry& AllottedGeometry, const float InAlpha)
{
    bMeshDirty = false;
    Vertices.Reset();
    Triangles.Reset();

    if (Texture == nullptr || !MeshFactory.IsValid())
    {
        return;
    }

    FVector2D LocalSize = AllottedGeometry.GetLocalSize();

    FVertexHelper Helper;
    Helper.ContentRect = FBox2D(FVector2D::ZeroVector, LocalSize);
    Helper.UVRect = Texture->UVRect;
    Helper.TextureSize = Texture->GetSize();
    if (Flip != EFlipType::None)
    {
        if (Flip == EFlipType::Horizontal || Flip == EFlipType::Both)
        {
            float tmp = Helper.UVRect.Min.X;
            Helper.UVRect.Min.X = Helper.UVRect.Max.X;
            Helper.UVRect.Max.X = tmp;
        }

        if (Flip == EFlipType::Vertical || Flip == EFlipType::Both)
        {
            float tmp = Helper.UVRect.Min.Y;
            Helper.UVRect.Min.Y = Helper.UVRect.Max.Y;
            Helper.UVRect.Max.Y = tmp;
        }
    }
    Helper.VertexColor = Color;
    MeshFactory->OnPopulateMesh(Helper);

    int32 vertCount = Helper.GetVertexCount();
    if (vertCount == 0)
    {
        return;
    }

    if (Texture->bRotated)
    {
        float xMin = Texture->UVRect.Min.X;
        float yMin = Texture->UVRect.Min.Y;
        float xMax = Texture->UVRect.Max.X;
        float yMax = Texture->UVRect.Max.Y;
        for (int32 i = 0; i < vertCount; i++)
        {
            auto& vec = Helper.Vertices[i].TexCoords;
            float tmp = vec[1];
            vec[1] = yMin + xMax - vec[0];
            vec[0] = xMin + tmp - yMin;
        }
    }

    Vertices += Helper.Vertices;
    Triangles += Helper.Triangles;

    int32 VerticeLength = Vertices.Num();
    for (int32 i = 0; i < VerticeLength; i++)
    {
        FSlateVertex& Vertex = Vertices[i];
        Vertex.Position = AllottedGeometry.LocalToAbsolute(Vertex.Position);
        Vertex.Color.A = FMath::Clamp<uint8>(Vertex.Color.A * InAlpha, 0, 255);
    }
}

void FNGraphics::PopulateDefaultMesh(FVertexHelper& Helper)
{
    FBox2D rect = Texture->GetDrawRect(Helper.ContentRect);

    Helper.AddQuad(rect, Helper.VertexColor, Helper.UVRect);
    Helper.AddTriangles();
}

void FNGraphics::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (Texture != nullptr)
    {
        Collector.AddReferencedObject(Texture);
    }
}