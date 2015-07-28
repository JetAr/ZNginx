﻿////////////////////////////////////////////////////////////////////////////////
// Filename: reflectionshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _REFLECTIONSHADERCLASS_H_
#define _REFLECTIONSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ReflectionShaderClass
////////////////////////////////////////////////////////////////////////////////
class ReflectionShaderClass
{
private:
    struct MatrixBufferType
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct ClipPlaneBufferType
    {
        D3DXVECTOR4 clipPlane;
    };

    struct LightBufferType
    {
        D3DXVECTOR4 lightDiffuseColor;
        D3DXVECTOR3 lightDirection;
        float colorTextureBrightness;
    };

public:
    ReflectionShaderClass();
    ReflectionShaderClass(const ReflectionShaderClass&);
    ~ReflectionShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
                D3DXVECTOR3, float, D3DXVECTOR4);

private:
    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
                             D3DXVECTOR3, float, D3DXVECTOR4);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* m_clipPlaneBuffer;
    ID3D11Buffer* m_lightBuffer;
};

#endif
