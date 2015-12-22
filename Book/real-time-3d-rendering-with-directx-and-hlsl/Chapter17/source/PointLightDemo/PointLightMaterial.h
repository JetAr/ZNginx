﻿#pragma once

#include "Common.h"
#include "Material.h"

using namespace Library;

namespace Rendering
{
typedef struct _PointLightMaterialVertex
{
    XMFLOAT4 Position;
    XMFLOAT2 TextureCoordinates;
    XMFLOAT3 Normal;

    _PointLightMaterialVertex() { }

    _PointLightMaterialVertex(XMFLOAT4 position, XMFLOAT2 textureCoordinates, XMFLOAT3 normal)
        : Position(position), TextureCoordinates(textureCoordinates), Normal(normal) { }
} PointLightMaterialVertex;

class PointLightMaterial : public Material
{
    RTTI_DECLARATIONS(PointLightMaterial, Material)

    MATERIAL_VARIABLE_DECLARATION(WorldViewProjection)
    MATERIAL_VARIABLE_DECLARATION(World)
    MATERIAL_VARIABLE_DECLARATION(SpecularColor)
    MATERIAL_VARIABLE_DECLARATION(SpecularPower)
    MATERIAL_VARIABLE_DECLARATION(AmbientColor)
    MATERIAL_VARIABLE_DECLARATION(LightColor)
    MATERIAL_VARIABLE_DECLARATION(LightPosition)
    MATERIAL_VARIABLE_DECLARATION(LightRadius)
    MATERIAL_VARIABLE_DECLARATION(CameraPosition)
    MATERIAL_VARIABLE_DECLARATION(ColorTexture)

public:
    PointLightMaterial();

    virtual void Initialize(Effect* effect) override;
    virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const override;
    void CreateVertexBuffer(ID3D11Device* device, PointLightMaterialVertex* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
    virtual UINT VertexSize() const override;
};
}

