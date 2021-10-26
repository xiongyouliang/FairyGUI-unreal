#pragma once

#include "UI/FairyComponent.h"
#include "FairyRoot.generated.h"

class UGGraph;
class UGWindow;

UCLASS(BlueprintType, NotBlueprintable)
class FAIRYGUI_API UFairyRoot : public UFairyComponent
{
    GENERATED_BODY()

public:
    static int32 ContentScaleLevel;

    UFairyRoot();
    virtual ~UFairyRoot();

    // ~ UGVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

    // ~ UObject Interface
    virtual void BeginDestroy() override;

    void ShowWindow(UGWindow* Window);
    void HideWindow(UGWindow* Window);
    void HideWindowImmediately(UGWindow* Window);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void BringToFront(UGWindow* Window);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ShowModalWait();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void CloseModalWait();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void CloseAllExceptModals();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void CloseAllWindows();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    UGWindow* GetTopWindow() const;

    UFairyObject* GetModalWaitingPane();
    UGGraph* GetModalLayer();

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool HasModalWindow() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool IsModalWaiting() const;

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ShowPopup(UFairyObject* Popup, UFairyObject* AtObject, EPopupDirection Direction);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void TogglePopup(UFairyObject* Popup, UFairyObject* AtObject = nullptr, EPopupDirection Direction = EPopupDirection::Auto);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void HidePopup(UFairyObject* Popup = nullptr);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    bool HasAnyPopup() const;

    FVector2D GetPoupPosition(UFairyObject* Popup, UFairyObject* AtObject, EPopupDirection Direction);
    void CheckPopups(SWidget* ClickTarget);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ShowTooltips(const FString& Text);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void ShowTooltipsWin(UFairyObject* InTooltipWin);

    UFUNCTION(BlueprintCallable, Category = "FairyGUI")
    void HideTooltips();

private:
    TSharedPtr<SWidget> FullScreenWidget;

    void AddToViewport();
    void RemoveFromViewport();

    void CreateModalLayer();
    void AdjustModalLayer();
    void ClosePopup(UFairyObject* Popup);

    //void UdateContentScaleLevel();

    void DoShowTooltipsWin();

    UPROPERTY(Transient)
    UGGraph* ModalLayer;

    UPROPERTY(Transient)
    UFairyObject* ModalWaitPane;

    UPROPERTY(Transient)
    UFairyObject* TooltipWin;

    UPROPERTY(Transient)
    UFairyObject* DefaultTooltipWin;

    TArray<UFairyObject*> PopupStack;
    TArray<UFairyObject*> JustClosedPopups;
    FTimerHandle ShowTooltipsTimerHandle;

    friend class UFairyApplication;
};
