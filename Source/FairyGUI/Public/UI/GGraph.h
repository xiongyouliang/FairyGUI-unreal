#pragma once

#include "UI/FairyObject.h"
#include "GGraph.generated.h"

UCLASS(BlueprintType)
class FAIRYGUI_API UGGraph : public UFairyObject
{
    GENERATED_BODY()

public:
    UGGraph();
    virtual ~UGGraph();

    virtual FColor NativeGetColor() const override;
    virtual void NativeSetColor(const FColor& InColor) override;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "LineColor,FillColor"))
    void DrawRect(float LineWidth, const FColor& LineColor, const FColor& FillColor);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "LineColor,FillColor"))
    void DrawRoundRect(float LineWidth, const FColor& LineColor, const FColor& FillColor, float TopLeftRadius, float TopRightRadius, float BottomLeftRadius, float BottomRightRadius);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "LineColor,FillColor"))
    void DrawEllipse(float LineWidth, const FColor& LineColor, const FColor& FillColor, float StartDegree = 0, float EndDegree = 360);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "LineColor,FillColor,Points"))
    void DrawPolygon(float LineWidth, const FColor& LineColor, const FColor& FillColor, const TArray<FVector2D>& Points);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI", meta = (AutoCreateRefTerm = "LineColor,FillColor,Distances"))
    void DrawRegularPolygon(int32 Sides, float LineWidth, const FColor& LineColor, const FColor& FillColor, float Rotation, const TArray<float>& Distances);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void Clear();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsEmpty() const;

    virtual FNVariant GetProp(EObjectPropID PropID) const override;
    virtual void SetProp(EObjectPropID PropID, const FNVariant& InValue) override;

protected:
    virtual void SetupBeforeAdd(FairyGUI::FByteBuffer* Buffer, int32 BeginPos) override;

private:
    TSharedPtr<class SShape> Content;
};