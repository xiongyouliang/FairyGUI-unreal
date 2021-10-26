#pragma once

#include "SDisplayObject.h"
#include "UI/FieldTypes.h"
#include "Mesh/FillMesh.h"
#include "NTexture.h"
#include "NGraphics.h"

class FAIRYGUI_API SFImage : public SDisplayObject, public IMeshFactory
{
public:
    SLATE_BEGIN_ARGS(SFImage) :
        _GObject(nullptr)
    {}
        SLATE_ARGUMENT(UFairyObject*, GObject)
    SLATE_END_ARGS()

    

    SFImage();
    ~SFImage();

	void Construct(const FArguments& InArgs);

    void SetTexture(UNTexture* InTexture);
    UNTexture* GetTexture() const { return Graphics->GetTexture();  }
    void SetScale9Grid(const TOptional<FBox2D>& GridRect);
    void SetScaleByTile(bool bInScaleByTile);
    void SetTileGridIndice(int32 InTileGridIndex);

    EFillMethod GetFillMethod() const;
    void SetFillMethod(EFillMethod Method);
    int32 GetFillOrigin() const;
    void SetFillOrigin(int32 Origin);
    bool IsFillClockwise() const;
    void SetFillClockwise(bool bClockwise);
    float GetFillAmount() const;
    void SetFillAmount(float Amount);

    FNGraphics *Graphics;
public:

	// SWidget overrides
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    // IMeshFactory overrides
    MESH_FACTORY_TYPE(SFImage)
    virtual void OnPopulateMesh(FVertexHelper& Helper) override;

protected:
    void TileFill(FVertexHelper& Helper, const FBox2D& ContentRect, const FBox2D& UVRect, const FVector2D& TextureSize);
    void SliceFill(FVertexHelper& Helper);

protected:
    TOptional<FBox2D> Scale9Grid;
    bool bScaleByTile;
    FVector2D TextureScale;
    int32 TileGridIndice;
    TUniquePtr<FFillMesh> FillMesh;
};