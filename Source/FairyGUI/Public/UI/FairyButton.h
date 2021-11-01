#pragma once

#include "UI/FairyComponent.h"
#include "FairyButton.generated.h"

class UGController;
class UGTextField;

UCLASS(BlueprintType, Blueprintable)
class FAIRYGUI_API UFairyButton : public UFairyComponent
{
    GENERATED_BODY()

public:
    UFairyButton();
    virtual ~UFairyButton();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetTitle() const { return Title; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetTitle(const FString& InTitle) { SetText(InTitle); };

    virtual const FString& GetText() const override { return Title; }
    virtual void SetText(const FString& InText) override;

    virtual const FString& GetIcon() const override;
    virtual void SetIcon(const FString& InIcon) override;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetSelectedTitle() const { return SelectedTitle; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetSelectedTitle(const FString& InTitle);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    const FString& GetSelectedIcon() const { return SelectedIcon; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetSelectedIcon(const FString& InIcon);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    FColor GetTitleColor() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetTitleColor(const FColor& InColor);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    int32 GetTitleFontSize() const;
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetTitleFontSize(int32 InFontSize);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsSelected() const { return bSelected; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UGController* GetRelatedController() const { return RelatedController; }
    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void SetRelatedController(UGController* InController);

    UGTextField* GetTextField() const;

    virtual FNVariant GetProp(EObjectPropID PropID) const override;
    virtual void SetProp(EObjectPropID PropID, const FNVariant& InValue) override;

    UPROPERTY(BlueprintAssignable, Category = "FairyGUI|Event")
    FGUIEventDynMDelegate OnChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FairyGUI")
    bool bChangeStateOnClick;

    static const FString UP;
    static const FString DOWN;
    static const FString OVER;
    static const FString SELECTED_OVER;
    static const FString DISABLED;
    static const FString SELECTED_DISABLED;

protected:
    virtual void ConstructExtension(FByteBuffer* Buffer) override;
    virtual void SetupAfterAdd(FByteBuffer* Buffer, int32 BeginPos) override;

    virtual void ApplyController(UGController* Controller) override;

    void SetState(const FString& InState);
    void SetCurrentState();

private:
    void OnRollOverHandler(UEventContext* Context);
    void OnRollOutHandler(UEventContext* Context);
    void OnTouchBeginHandler(UEventContext* Context);
    void OnTouchEndHandler(UEventContext* Context);
    void OnClickHandler(UEventContext* Context);
    void OnRemovedFromStageHandler(UEventContext* Context);

    EButtonMode Mode;

    UPROPERTY()
    UFairyObject* TitleObject;
    UPROPERTY()
    UFairyObject* IconObject;
    UPROPERTY()
    UGController* ButtonController;
    UPROPERTY()
    UGController* RelatedController;

    FString RelatedPageID;
    FString Title;
    FString SelectedTitle;
    FString Icon;
    FString SelectedIcon;
    FString Sound;
    float SoundVolumeScale;
    bool bSelected;
    bool bOver;
    bool bDown;
    EButtonPressedEffect DownEffect;
    bool bDownScaled;
    float DownEffectValue;
};