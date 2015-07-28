////////////////////////////////////////////////////////////////////////////////
// Filename: glassshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GLASSSHADERCLASS_H_
#define _GLASSSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: GlassShaderClass
////////////////////////////////////////////////////////////////////////////////
class GlassShaderClass
{
public:
    GlassShaderClass();
    GlassShaderClass(const GlassShaderClass&);
    ~GlassShaderClass();

    bool Initialize(ID3D10Device*, HWND);
    void Shutdown();
    void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*,
                ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, float);

private:
    bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*,
                             ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, float);
    void RenderShader(ID3D10Device*, int);

private:
    ID3D10Effect* m_effect;
    ID3D10EffectTechnique* m_technique;
    ID3D10InputLayout* m_layout;

    ID3D10EffectMatrixVariable* m_worldMatrixPtr;
    ID3D10EffectMatrixVariable* m_viewMatrixPtr;
    ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

    ID3D10EffectShaderResourceVariable* m_colorTexturePtr;
    ID3D10EffectShaderResourceVariable* m_normalTexturePtr;
    ID3D10EffectShaderResourceVariable* m_refractionTexturePtr;

    ID3D10EffectScalarVariable* m_refractionScalePtr;
};

#endif
