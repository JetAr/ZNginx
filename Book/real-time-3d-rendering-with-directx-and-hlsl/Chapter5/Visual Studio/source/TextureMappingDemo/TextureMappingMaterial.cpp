﻿#include "TextureMappingMaterial.h"
#include "GameException.h"
#include "Mesh.h"

namespace Rendering
{
RTTI_DEFINITIONS(TextureMappingMaterial)

TextureMappingMaterial::TextureMappingMaterial()
    : Material("main10"),
      MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection), MATERIAL_VARIABLE_INITIALIZATION(ColorTexture)
{
}

MATERIAL_VARIABLE_DEFINITION(TextureMappingMaterial, WorldViewProjection)
MATERIAL_VARIABLE_DEFINITION(TextureMappingMaterial, ColorTexture)

void TextureMappingMaterial::Initialize(Effect& effect)
{
    Material::Initialize(effect);

    MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection)
    MATERIAL_VARIABLE_RETRIEVE(ColorTexture)

    D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    CreateInputLayout("main10", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));
}

void TextureMappingMaterial::CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
{
    const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
    std::vector<XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
    assert(textureCoordinates->size() == sourceVertices.size());

    std::vector<VertexPositionTexture> vertices;
    vertices.reserve(sourceVertices.size());
    for (UINT i = 0; i < sourceVertices.size(); i++)
    {
        XMFLOAT3 position = sourceVertices.at(i);
        XMFLOAT3 uv = textureCoordinates->at(i);
        vertices.push_back(VertexPositionTexture(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y)));
    }

    CreateVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);
}

void TextureMappingMaterial::CreateVertexBuffer(ID3D11Device* device, VertexPositionTexture* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
{
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = VertexSize() * vertexCount;
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubResourceData;
    ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
    vertexSubResourceData.pSysMem = vertices;
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
    {
        throw GameException("ID3D11Device::CreateBuffer() failed.");
    }
}

UINT TextureMappingMaterial::VertexSize() const
{
    return sizeof(VertexPositionTexture);
}
}
