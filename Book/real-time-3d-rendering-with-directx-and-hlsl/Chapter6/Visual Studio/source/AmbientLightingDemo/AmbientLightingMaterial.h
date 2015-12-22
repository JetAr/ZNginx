﻿#pragma once

#include "Common.h"
#include "Material.h"
#include "VertexDeclarations.h"

using namespace Library;

namespace Rendering
{
class AmbientLightingMaterial : public Material
{
    RTTI_DECLARATIONS(AmbientLightingMaterial, Material)

    MATERIAL_VARIABLE_DECLARATION(WorldViewProjection)
    MATERIAL_VARIABLE_DECLARATION(ColorTexture)
    MATERIAL_VARIABLE_DECLARATION(AmbientColor)

public:
    AmbientLightingMaterial();

    virtual void Initialize(Effect& effect) override;
    virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const override;
    void CreateVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
    virtual UINT VertexSize() const override;
};
}

