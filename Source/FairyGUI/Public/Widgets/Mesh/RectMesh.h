#pragma once

#include "MeshFactory.h"

class FAIRYGUI_API FRectMesh : public IMeshFactory
{
public:
    MESH_FACTORY_TYPE(FRectMesh)

    FRectMesh();
    virtual ~FRectMesh() {}

    TOptional<FBox2D> DrawRect;
    float LineWidth;
    FColor LineColor;
    TOptional<FColor> FillColor;
    TOptional<TArray<FColor>> Colors;

    void OnPopulateMesh(FVertexHelper& Helper);
};