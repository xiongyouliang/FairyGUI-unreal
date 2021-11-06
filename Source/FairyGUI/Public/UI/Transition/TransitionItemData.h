#pragma once

#include "CoreMinimal.h"

struct FTransitionItemData {
  float f1;
  float f2;
  float f3;
  float f4;
  bool b1;
  bool b2;
  bool b3;

  FTransitionItemData();
  FVector2D GetVec2() const;
  void SetVec2(const FVector2D &value);
  FVector4 GetVec4() const;
  void SetVec4(const FVector4 &value);
  FColor GetColor() const;
  void SetColor(const FColor &value);
};