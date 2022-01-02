#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/NVariant.h"
#include "UI/Tween/FairyEaseType.h"
#include "UI/FairyConfig.h"
#include "FairyBlueprintLibrary.generated.h"

UCLASS()
class FAIRYGUI_API UFairyBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static UFairyConfig* GetUIConfig();

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void SetUIConfig(UFairyConfig* InConfig);

	static void SetFairyConfigFontRes(FString url);
	static void SetFairyConfigHScrollBar(FString url);
	static void SetFairyConfigVScrollBar(FString url);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant")
	static bool GetVariantAsBool(UPARAM(ref) FNVariant& InVariant);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant")
	static int32 GetVariantAsInt(UPARAM(ref) FNVariant& InVariant);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant")
	static float GetVariantAsFloat(UPARAM(ref) FNVariant& InVariant);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant")
	static FString GetVariantAsString(UPARAM(ref) FNVariant& InVariant);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant")
	static FColor GetVariantAsColor(UPARAM(ref) FNVariant& InVariant);

	UFUNCTION(BlueprintPure, Category = "FairyGUI|Variant", meta = (DeterminesOutputType = "ClassType"))
	static UObject* GetVariantAsUObject(UPARAM(ref) FNVariant& InVariant, TSubclassOf<UObject> ClassType);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantBool(UPARAM(ref) FNVariant& InVariant, bool bInValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantInt(UPARAM(ref) FNVariant& InVariant, int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantFloat(UPARAM(ref) FNVariant& InVariant, float InValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantString(UPARAM(ref) FNVariant& InVariant, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantColor(UPARAM(ref) FNVariant& InVariant, const FColor& InValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI|Variant")
	static FNVariant& SetVariantUObject(UPARAM(ref) FNVariant& InVariant, UObject* InValue);

	UFUNCTION(BlueprintCallable, Category = "FairyGUI")
	static void SetPackageItemExtension(const FString& URL, TSubclassOf<UFairyComponent> ClassType);
};