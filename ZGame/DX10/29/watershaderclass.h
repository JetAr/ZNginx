////////////////////////////////////////////////////////////////////////////////
// Filename: watershaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WATERSHADERCLASS_H_
#define _WATERSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: WaterShaderClass
////////////////////////////////////////////////////////////////////////////////
class WaterShaderClass
{
public:
    WaterShaderClass();
    WaterShaderClass(const WaterShaderClass&);
    ~WaterShaderClass();

    bool Initialize(ID3D10Device*, HWND);
    void Shutdown();
    void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*,
                ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, float, float);

private:
    bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*,
                             ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, float, float);
    void RenderShader(ID3D10Device*, int);

private:
    ID3D10Effect* m_effect;
    ID3D10EffectTechnique* m_technique;
    ID3D10InputLayout* m_layout;

    ID3D10EffectMatrixVariable* m_worldMatrixPtr;
    ID3D10EffectMatrixVariable* m_viewMatrixPtr;
    ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

    ID3D10EffectMatrixVariable* m_reflectionMatrixPtr;
    ID3D10EffectShaderResourceVariable* m_reflectionTexturePtr;
    ID3D10EffectShaderResourceVariable* m_refractionTexturePtr;
    ID3D10EffectShaderResourceVariable* m_normalTexturePtr;
    ID3D10EffectScalarVariable* m_translationPtr;
    ID3D10EffectScalarVariable* m_reflectRefractScalePtr;
};

#endif
