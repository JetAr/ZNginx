////////////////////////////////////////////////////////////////////////////////
// Filename: fireshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FIRESHADERCLASS_H_
#define _FIRESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: FireShaderClass
////////////////////////////////////////////////////////////////////////////////
class FireShaderClass
{
public:
    FireShaderClass();
    FireShaderClass(const FireShaderClass&);
    ~FireShaderClass();

    bool Initialize(ID3D10Device*, HWND);
    void Shutdown();
    void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, ID3D10ShaderResourceView*,
                ID3D10ShaderResourceView*, float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:
    bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, ID3D10ShaderResourceView*,
                             ID3D10ShaderResourceView*, float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2,
                             float, float);
    void RenderShader(ID3D10Device*, int);

private:
    ID3D10Effect* m_effect;
    ID3D10EffectTechnique* m_technique;
    ID3D10InputLayout* m_layout;

    ID3D10EffectMatrixVariable* m_worldMatrixPtr;
    ID3D10EffectMatrixVariable* m_viewMatrixPtr;
    ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

    ID3D10EffectShaderResourceVariable* m_fireTexturePtr;
    ID3D10EffectShaderResourceVariable* m_noiseTexturePtr;
    ID3D10EffectShaderResourceVariable* m_alphaTexturePtr;

    ID3D10EffectScalarVariable* m_frameTimePtr;
    ID3D10EffectVectorVariable* m_scrollSpeedsPtr;
    ID3D10EffectVectorVariable* m_scalesPtr;
    ID3D10EffectVectorVariable* m_distortion1Ptr;
    ID3D10EffectVectorVariable* m_distortion2Ptr;
    ID3D10EffectVectorVariable* m_distortion3Ptr;
    ID3D10EffectScalarVariable* m_distortionScalePtr;
    ID3D10EffectScalarVariable* m_distortionBiasPtr;
};

#endif
