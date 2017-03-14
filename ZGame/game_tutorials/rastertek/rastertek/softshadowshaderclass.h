﻿////////////////////////////////////////////////////////////////////////////////
// Filename: softshadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SOFTSHADOWSHADERCLASS_H_
#define _SOFTSHADOWSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SoftShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class SoftShadowShaderClass
{
private:
    struct MatrixBufferType
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct LightBufferType
    {
        D3DXVECTOR4 ambientColor;
        D3DXVECTOR4 diffuseColor;
    };

    struct LightBufferType2
    {
        D3DXVECTOR3 lightPosition;
        float padding;
    };

public:
    SoftShadowShaderClass();
    SoftShadowShaderClass(const SoftShadowShaderClass&);
    ~SoftShadowShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3,
                D3DXVECTOR4, D3DXVECTOR4);

private:
    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3,
                             D3DXVECTOR4, D3DXVECTOR4);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleStateWrap;
    ID3D11SamplerState* m_sampleStateClamp;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* m_lightBuffer;
    ID3D11Buffer* m_lightBuffer2;
};

#endif
