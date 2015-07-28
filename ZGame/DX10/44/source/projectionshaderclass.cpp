////////////////////////////////////////////////////////////////////////////////
// Filename: projectionshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "projectionshaderclass.h"


ProjectionShaderClass::ProjectionShaderClass()
{
    m_effect = 0;
    m_technique = 0;
    m_layout = 0;

    m_worldMatrixPtr = 0;
    m_viewMatrixPtr = 0;
    m_projectionMatrixPtr = 0;

    m_texturePtr = 0;

    m_ambientColorPtr = 0;
    m_diffuseColorPtr = 0;
    m_lightPositionPtr = 0;

    m_viewMatrix2Ptr = 0;
    m_projectionMatrix2Ptr = 0;
    m_projectionTexturePtr = 0;
}


ProjectionShaderClass::ProjectionShaderClass(const ProjectionShaderClass& other)
{
}


ProjectionShaderClass::~ProjectionShaderClass()
{
}


bool ProjectionShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
    bool result;


    // Initialize the shader that will be used to draw the triangles.
    result = InitializeShader(device, hwnd, L"../Engine/projection.fx");
    if(!result)
    {
        return false;
    }

    return true;
}


void ProjectionShaderClass::Shutdown()
{
    // Shutdown the shader effect.
    ShutdownShader();

    return;
}


void ProjectionShaderClass::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                                   ID3D10ShaderResourceView* texture, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightPosition,
                                   D3DXMATRIX viewMatrix2, D3DXMATRIX projectionMatrix2, ID3D10ShaderResourceView* projectionTexture)
{
    // Set the shader parameters that it will use for rendering.
    SetShaderParameters(device, worldMatrix, viewMatrix, projectionMatrix, texture, ambientColor, diffuseColor, lightPosition, viewMatrix2,
                        projectionMatrix2, projectionTexture);

    // Now render the prepared buffers with the shader.
    RenderShader(device, indexCount);

    return;
}


bool ProjectionShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
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
    m_technique = m_effect->GetTechniqueByName("ProjectionTechnique");
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

    // Get pointers to the texture resource inside the shader.
    m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();

    // Get pointers to the light variables inside the shader.
    m_ambientColorPtr = m_effect->GetVariableByName("ambientColor")->AsVector();
    m_diffuseColorPtr = m_effect->GetVariableByName("diffuseColor")->AsVector();
    m_lightPositionPtr = m_effect->GetVariableByName("lightPosition")->AsVector();

    // Set the projection related resources in the shader.
    m_viewMatrix2Ptr = m_effect->GetVariableByName("viewMatrix2")->AsMatrix();
    m_projectionMatrix2Ptr = m_effect->GetVariableByName("projectionMatrix2")->AsMatrix();
    m_projectionTexturePtr = m_effect->GetVariableByName("projectionTexture")->AsShaderResource();

    return true;
}


void ProjectionShaderClass::ShutdownShader()
{
    // Release the projection related pointers.
    m_viewMatrix2Ptr = 0;
    m_projectionMatrix2Ptr = 0;
    m_projectionTexturePtr = 0;

    // Release the light pointers.
    m_ambientColorPtr = 0;
    m_diffuseColorPtr = 0;
    m_lightPositionPtr = 0;

    // Release the pointer to the texture in the shader file.
    m_texturePtr = 0;

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


void ProjectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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


void ProjectionShaderClass::SetShaderParameters(ID3D10Device* device, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
        ID3D10ShaderResourceView* texture, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
        D3DXVECTOR3 lightPosition, D3DXMATRIX viewMatrix2, D3DXMATRIX projectionMatrix2,
        ID3D10ShaderResourceView* projectionTexture)
{
    // Set the world matrix variable inside the shader.
    m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);

    // Set the view matrix variable inside the shader.
    m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);

    // Set the projection matrix variable inside the shader.
    m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

    // Bind the color texture.
    m_texturePtr->SetResource(texture);

    // Set the light variables inside the shader.
    m_ambientColorPtr->SetFloatVector((float*)&ambientColor);
    m_diffuseColorPtr->SetFloatVector((float*)&diffuseColor);
    m_lightPositionPtr->SetFloatVector((float*)&lightPosition);

    // Set the projection related variables inside the shader.
    m_viewMatrix2Ptr->SetMatrix((float*)&viewMatrix2);
    m_projectionMatrix2Ptr->SetMatrix((float*)&projectionMatrix2);
    m_projectionTexturePtr->SetResource(projectionTexture);

    return;
}


void ProjectionShaderClass::RenderShader(ID3D10Device* device, int indexCount)
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
