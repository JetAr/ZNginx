////////////////////////////////////////////////////////////////////////////////
// Filename: specmapshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SPECMAPSHADERCLASS_H_
#define _SPECMAPSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SpecMapShaderClass
////////////////////////////////////////////////////////////////////////////////
class SpecMapShaderClass
{
public:
    SpecMapShaderClass();
    SpecMapShaderClass(const SpecMapShaderClass&);
    ~SpecMapShaderClass();

    bool Initialize(ID3D10Device*, HWND);
    void Shutdown();
    void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4,
                D3DXVECTOR3, D3DXVECTOR4, float);

private:
    bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4,
                             D3DXVECTOR3, D3DXVECTOR4, float);
    void RenderShader(ID3D10Device*, int);

private:
    ID3D10Effect* m_effect;
    ID3D10EffectTechnique* m_technique;
    ID3D10InputLayout* m_layout;

    ID3D10EffectMatrixVariable* m_worldMatrixPtr;
    ID3D10EffectMatrixVariable* m_viewMatrixPtr;
    ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

    ID3D10EffectShaderResourceVariable* m_textureArrayPtr;
    ID3D10EffectVectorVariable* lightDirectionPtr;
    ID3D10EffectVectorVariable* diffuseColorPtr;

    ID3D10EffectVectorVariable* cameraPositionPtr;
    ID3D10EffectVectorVariable* specularColorPtr;
    ID3D10EffectScalarVariable* specularPowerPtr;
};

#endif
