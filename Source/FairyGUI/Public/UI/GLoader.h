#pragma once

#include "UI/FairyObject.h"
#include "GLoader.generated.h"

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UGLoader : public UFairyObject
{
    GENERATED_BODY()

public:
    UGLoader();
    virtual ~UGLoader();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetURL() const { return URL; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetURL(const FString& InURL);

    virtual const FString& GetIcon() const override { return URL; }
    virtual void SetIcon(const FString& InIcon) override { SetURL(InIcon); }

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    EHAlignType GetAlign() const { return Align; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetAlign(EHAlignType InAlign);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    EVAlignType GetVerticalAlign() const { return VerticalAlign; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetVerticalAlign(EVAlignType InVerticalAlign);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool GetAutoSize() const { return bAutoSize; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetAutoSize(bool bInAutoSize);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    ELoaderFillType GetFill() const { return Fill; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFill(ELoaderFillType InFillType);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsShrinkOnly() const { return bShrinkOnly; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetShrinkOnly(bool bInShrinkOnly);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    EFlipType GetFlip() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFlip(EFlipType InFlip);

    virtual FColor NativeGetColor() const override;
    virtual void NativeSetColor(const FColor& InColor) override;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    EFillMethod GetFillMethod() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFillMethod(EFillMethod Method);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    int32 GetFillOrigin() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFillOrigin(int32 Origin);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsFillClockwise() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFillClockwise(bool bClockwise);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    float GetFillAmount() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFillAmount(float Amount);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsPlaying() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetPlaying(bool bInPlaying);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    int32 GetFrame() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetFrame(int32 InFrame);

    virtual FNVariant GetProp(EObjectPropID PropID) const;
    virtual void SetProp(EObjectPropID PropID, const FNVariant& InValue);

protected:
    //virtual void HandleSizeChanged() override;
    virtual void SetupBeforeAdd(FByteBuffer* Buffer, int32 BeginPos) override;

    void LoadContent();
    void ClearContent();
    void LoadFromPackage(const FString& ItemURL);
    void LoadExternal();
    void UpdateLayout();
    void SetErrorState();

private:
    TSharedPtr<class SContainer> Container;
    TSharedPtr<class SMovieClip> Content;
    UPROPERTY(Transient)
    UFairyObject* Content2;
    TSharedPtr<FFairyPackageItem> ContentItem;
    FString URL;
    ELoaderFillType Fill;
    EHAlignType Align;
    EVAlignType VerticalAlign;
    bool bShowErrorSign;
    bool bShrinkOnly;
    bool bAutoSize;
    bool bUpdatingLayout;
};