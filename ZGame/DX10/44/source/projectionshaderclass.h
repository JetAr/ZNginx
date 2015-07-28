////////////////////////////////////////////////////////////////////////////////
// Filename: projectionshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PROJECTIONSHADERCLASS_H_
#define _PROJECTIONSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ProjectionShaderClass
////////////////////////////////////////////////////////////////////////////////
class ProjectionShaderClass
{
public:
    ProjectionShaderClass();
    ProjectionShaderClass(const ProjectionShaderClass&);
    ~ProjectionShaderClass();

    bool Initialize(ID3D10Device*, HWND);
    void Shutdown();
    void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXMATRIX,
                D3DXMATRIX, ID3D10ShaderResourceView*);

private:
    bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    void SetShaderParameters(ID3D10Device*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3,
                             D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*);
    void RenderShader(ID3D10Device*, int);

private:
    ID3D10Effect* m_effect;
    ID3D10EffectTechnique* m_technique;
    ID3D10InputLayout* m_layout;

    ID3D10EffectMatrixVariable* m_worldMatrixPtr;
    ID3D10EffectMatrixVariable* m_viewMatrixPtr;
    ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

    ID3D10EffectShaderResourceVariable* m_texturePtr;

    ID3D10EffectVectorVariable* m_ambientColorPtr;
    ID3D10EffectVectorVariable* m_diffuseColorPtr;
    ID3D10EffectVectorVariable* m_lightPositionPtr;

    ID3D10EffectMatrixVariable* m_viewMatrix2Ptr;
    ID3D10EffectMatrixVariable* m_projectionMatrix2Ptr;
    ID3D10EffectShaderResourceVariable* m_projectionTexturePtr;
};

#endif
