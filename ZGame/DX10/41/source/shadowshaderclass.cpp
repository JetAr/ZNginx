////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadowshaderclass.h"


ShadowShaderClass::ShadowShaderClass()
{
    m_effect = 0;
    m_technique = 0;
    m_layout = 0;

    m_worldMatrixPtr = 0;
    m_viewMatrixPtr = 0;
    m_projectionMatrixPtr = 0;

    m_lightViewMatrixPtr = 0;
    m_lightProjectionMatrixPtr = 0;

    m_texturePtr = 0;
    m_depthMapTexturePtr = 0;

    m_lightPositionPtr = 0;
    m_ambientColorPtr = 0;
    m_diffuseColorPtr = 0;

    m_lightViewMatrixPtr2 = 0;
    m_lightProjectionMatrixPtr2 = 0;
    m_depthMapTexturePtr2 = 0;
    m_lightPositionPtr2 = 0;
    m_diffuseColorPtr2 = 0;
}


ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass& other)
{
}


ShadowShaderClass::~ShadowShaderClass()
{
}


bool ShadowShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
    bool result;


    // Initialize the shader that will be used to draw the triangles.
    result = InitializeShader(device, hwnd, L"../Engine/shadow.fx");
    if(!result)
    {
        return false;
    }

    return true;
}


void ShadowShaderClass::Shutdown()
{
    // Shutdown the shader effect.
    ShutdownShader();

    return;
}


void ShadowShaderClass::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                               D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView* texture,
                               ID3D10ShaderResourceView* depthMapTexture, D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
                               D3DXMATRIX lightViewMatrix2, D3DXMATRIX lightProjectionMatrix2, ID3D10ShaderResourceView* depthMapTexture2,
                               D3DXVECTOR3 lightPosition2, D3DXVECTOR4 diffuseColor2)
{
    // Set the shader parameters that it will use for rendering.
    SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture,
                        lightPosition, ambientColor, diffuseColor, lightViewMatrix2, lightProjectionMatrix2, depthMapTexture2, lightPosition2,
                        diffuseColor2);

    // Now render the prepared buffers with the shader.
    RenderShader(device, indexCount);

    return;
}


bool ShadowShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    D3D10_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D10_PASS_DESC passDesc;


    // Initialize the error message.
    errorMessage = 0;

    // Load the shader in from the file.
    result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &m_effect, &errorMessage,
                                        NULL);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, filename);
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // Get a pointer to the technique inside the shader.
    m_technique = m_effect->GetTechniqueByName("ShadowTechnique");
    if(!m_technique)
    {
        return false;
    }

    // Now setup the layout of the data that goes into the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Get the description of the first pass described in the shader technique.
    m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

    // Create the input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
    if(FAILED(result))
    {
        return false;
    }

    // Get pointers to the three matrices inside the shader so we can update them from this class.
    m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
    m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
    m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

    // Get pointers to the light matrices inside the shader.
    m_lightViewMatrixPtr = m_effect->GetVariableByName("lightViewMatrix")->AsMatrix();
    m_lightProjectionMatrixPtr = m_effect->GetVariableByName("lightProjectionMatrix")->AsMatrix();

    // Get pointers to the texture resources inside the shader.
    m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();
    m_depthMapTexturePtr = m_effect->GetVariableByName("depthMapTexture")->AsShaderResource();

    // Get pointers to the light position and diffuse color variables inside the shader.
    m_lightPositionPtr = m_effect->GetVariableByName("lightPosition")->AsVector();
    m_ambientColorPtr = m_effect->GetVariableByName("ambientColor")->AsVector();
    m_diffuseColorPtr = m_effect->GetVariableByName("diffuseColor")->AsVector();

    // Setup up pointers to the secondary light info.
    m_lightViewMatrixPtr2 = m_effect->GetVariableByName("lightViewMatrix2")->AsMatrix();
    m_lightProjectionMatrixPtr2 = m_effect->GetVariableByName("lightProjectionMatrix2")->AsMatrix();
    m_depthMapTexturePtr2 = m_effect->GetVariableByName("depthMapTexture2")->AsShaderResource();
    m_lightPositionPtr2 = m_effect->GetVariableByName("lightPosition2")->AsVector();
    m_diffuseColorPtr2 = m_effect->GetVariableByName("diffuseColor2")->AsVector();

    return true;
}


void ShadowShaderClass::ShutdownShader()
{
    // Release the second light info pointers.
    m_lightViewMatrixPtr2 = 0;
    m_lightProjectionMatrixPtr2 = 0;
    m_depthMapTexturePtr2 = 0;
    m_lightPositionPtr2 = 0;
    m_diffuseColorPtr2 = 0;

    // Release the light pointers.
    m_lightPositionPtr = 0;
    m_ambientColorPtr = 0;
    m_diffuseColorPtr = 0;

    // Release the pointers to the textures in the shader file.
    m_texturePtr = 0;
    m_depthMapTexturePtr = 0;

    // Release the light matrices.
    m_lightViewMatrixPtr = 0;
    m_lightProjectionMatrixPtr = 0;

    // Release the pointers to the matrices inside the shader.
    m_worldMatrixPtr = 0;
    m_viewMatrixPtr = 0;
    m_projectionMatrixPtr = 0;

    // Release the pointer to the shader layout.
    if(m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pointer to the shader technique.
    m_technique = 0;

    // Release the pointer to the shader.
    if(m_effect)
    {
        m_effect->Release();
        m_effect = 0;
    }

    return;
}


void ShadowShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors;
    unsigned long bufferSize, i;
    ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for(i=0; i<bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

    return;
}


void ShadowShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix,
        D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView* texture, ID3D10ShaderResourceView* depthMapTexture,
        D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXMATRIX lightViewMatrix2,
        D3DXMATRIX lightProjectionMatrix2, ID3D10ShaderResourceView* depthMapTexture2, D3DXVECTOR3 lightPosition2,
        D3DXVECTOR4 diffuseColor2)
{
    // Set the world matrix variable inside the shader.
    m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);

    // Set the view matrix variable inside the shader.
    m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);

    // Set the projection matrix variable inside the shader.
    m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

    // Set the light matrices in the shader.
    m_lightViewMatrixPtr->SetMatrix((float*)&lightViewMatrix);
    m_lightProjectionMatrixPtr->SetMatrix((float*)&lightProjectionMatrix);

    // Bind the textures.
    m_texturePtr->SetResource(texture);
    m_depthMapTexturePtr->SetResource(depthMapTexture);

    // Set the light variables inside the shader.
    m_lightPositionPtr->SetFloatVector((float*)&lightPosition);
    m_ambientColorPtr->SetFloatVector((float*)&ambientColor);
    m_diffuseColorPtr->SetFloatVector((float*)&diffuseColor);

    // Set the second light pointers.
    m_lightViewMatrixPtr2->SetMatrix((float*)&lightViewMatrix2);
    m_lightProjectionMatrixPtr2->SetMatrix((float*)&lightProjectionMatrix2);
    m_depthMapTexturePtr2->SetResource(depthMapTexture2);
    m_lightPositionPtr2->SetFloatVector((float*)&lightPosition2);
    m_diffuseColorPtr2->SetFloatVector((float*)&diffuseColor2);

    return;
}


void ShadowShaderClass::RenderShader(ID3D10Device* device, int indexCount)
{
    D3D10_TECHNIQUE_DESC techniqueDesc;
    unsigned int i;


    // Set the input layout.
    device->IASetInputLayout(m_layout);

    // Get the description structure of the technique from inside the shader so it can be used for rendering.
    m_technique->GetDesc(&techniqueDesc);

    // Go through each pass in the technique (should be just one currently) and render the triangles.
    for(i=0; i<techniqueDesc.Passes; ++i)
    {
        m_technique->GetPassByIndex(i)->Apply(0);
        device->DrawIndexed(indexCount, 0, 0);
    }

    return;
}
