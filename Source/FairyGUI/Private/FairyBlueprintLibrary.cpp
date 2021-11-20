#include "FairyBlueprintLibrary.h"
#include "UI/FairyConfig.h"

UFairyConfig* UFairyBlueprintLibrary::GetUIConfig()
{
	return UFairyConfig::Get();
}

void UFairyBlueprintLibrary::SetUIConfig(UFairyConfig* InConfig)
{
	//UFairyConfig::Config = InConfig;
}

bool UFairyBlueprintLibrary::GetVariantAsBool(UPARAM(ref) FNVariant& InVariant)
{
	return InVariant.AsBool();
}

int32 UFairyBlueprintLibrary::GetVariantAsInt(UPARAM(ref) FNVariant& InVariant)
{
	return InVariant.AsInt();
}

float UFairyBlueprintLibrary::GetVariantAsFloat(UPARAM(ref) FNVariant& InVariant)
{
	return InVariant.AsFloat();
}

FString UFairyBlueprintLibrary::GetVariantAsString(UPARAM(ref) FNVariant& InVariant)
{
	return InVariant.AsString();
}

FColor UFairyBlueprintLibrary::GetVariantAsColor(UPARAM(ref) FNVariant& InVariant)
{
	return InVariant.AsColor();
}

UObject* UFairyBlueprintLibrary::GetVariantAsUObject(UPARAM(ref) FNVariant& InVariant, TSubclassOf<UObject> ClassType)
{
	return InVariant.AsUObject();
}

FNVariant& UFairyBlueprintLibrary::SetVariantBool(UPARAM(ref) FNVariant& InVariant, bool bInValue)
{
	InVariant = bInValue;
	return InVariant;
}

FNVariant& UFairyBlueprintLibrary::SetVariantInt(UPARAM(ref) FNVariant& InVariant, int32 InValue)
{
	InVariant = InValue;
	return InVariant;
}

FNVariant& UFairyBlueprintLibrary::SetVariantFloat(UPARAM(ref) FNVariant& InVariant, float InValue)
{
	InVariant = InValue;
	return InVariant;
}

FNVariant& UFairyBlueprintLibrary::SetVariantString(UPARAM(ref) FNVariant& InVariant, const FString& InValue)
{
	InVariant = InValue;
	return InVariant;
}

FNVariant& UFairyBlueprintLibrary::SetVariantColor(UPARAM(ref) FNVariant& InVariant, const FColor& InValue)
{
	InVariant = InValue;
	return InVariant;
}

FNVariant& UFairyBlueprintLibrary::SetVariantUObject(UPARAM(ref) FNVariant& InVariant, UObject* InValue)
{
	InVariant = (void*)InValue;
	return InVariant;
}

void UFairyBlueprintLibrary::SetPackageItemExtension(const FString& URL, TSubclassOf<UFairyComponent> ClassType)
{
	FUIObjectFactory::SetExtension(URL, ClassType);
}