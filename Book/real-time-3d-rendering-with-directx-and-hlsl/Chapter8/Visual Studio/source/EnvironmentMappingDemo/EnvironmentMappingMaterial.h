﻿#pragma once

#include "Common.h"
#include "Material.h"
#include "VertexDeclarations.h"

using namespace Library;

namespace Rendering
{
class EnvironmentMappingMaterial : public Material
{
    RTTI_DECLARATIONS(EnvironmentMappingMaterial, Material)

    MATERIAL_VARIABLE_DECLARATION(WorldViewProjection)
    MATERIAL_VARIABLE_DECLARATION(World)
    MATERIAL_VARIABLE_DECLARATION(AmbientColor)
    MATERIAL_VARIABLE_DECLARATION(EnvColor)
    MATERIAL_VARIABLE_DECLARATION(ReflectionAmount)
    MATERIAL_VARIABLE_DECLARATION(ColorTexture)
    MATERIAL_VARIABLE_DECLARATION(EnvironmentMap)
    MATERIAL_VARIABLE_DECLARATION(CameraPosition)

public:
    EnvironmentMappingMaterial();

    virtual void Initialize(Effect& effect) override;
    virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const override;
    void CreateVertexBuffer(ID3D11Device* device, VertexPositionTextureNormal* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
    virtual UINT VertexSize() const override;
};
}

