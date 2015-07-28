////////////////////////////////////////////////////////////////////////////////
// Filename: fireshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fireshaderclass.h"


FireShaderClass::FireShaderClass()
{
    m_effect = 0;
    m_technique = 0;
    m_layout = 0;

    m_worldMatrixPtr = 0;
    m_viewMatrixPtr = 0;
    m_projectionMatrixPtr = 0;

    m_fireTexturePtr = 0;
    m_noiseTexturePtr = 0;
    m_alphaTexturePtr = 0;

    m_frameTimePtr = 0;
    m_scrollSpeedsPtr = 0;
    m_scalesPtr = 0;
    m_distortion1Ptr = 0;
    m_distortion2Ptr = 0;
    m_distortion3Ptr = 0;
    m_distortionScalePtr = 0;
    m_distortionBiasPtr = 0;
}


FireShaderClass::FireShaderClass(const FireShaderClass& other)
{
}


FireShaderClass::~FireShaderClass()
{
}


bool FireShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
    bool result;


    // Initialize the shader that will be used to draw the triangles.
    result = InitializeShader(device, hwnd, L"../Engine/fire.fx");
    if(!result)
    {
        return false;
    }

    return true;
}


void FireShaderClass::Shutdown()
{
    // Shutdown the shader effect.
    ShutdownShader();

    return;
}


void FireShaderClass::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
                             D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* fireTexture,
                             ID3D10ShaderResourceView* noiseTexture, ID3D10ShaderResourceView* alphaTexture, float frameTime,
                             D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2,
                             D3DXVECTOR2 distortion3, float distortionScale, float distortionBias)
{
    // Set the shader parameters that will be used for rendering.
    SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture, alphaTexture, frameTime, scrollSpeeds,
                        scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);

    // Now render the prepared buffers with the shader.
    RenderShader(device, indexCount);

    return;
}


bool FireShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D10_PASS_DESC passDesc;


    // Initialize the error message.
    errorMessage = 0;

    // Load the shader in from the file.
    result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                        device, NULL, NULL, &m_effect, &errorMessage, NULL);
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
    m_technique = m_effect->GetTechniqueByName("FireTechnique");
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

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Get the description of the first pass described in the shader technique.
    m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

    // Create the input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
                                       &m_layout);
    if(FAILED(result))
    {
        return false;
    }

    // Get pointers to the three matrices inside the shader so they can be updated from this class.
    m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
    m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
    m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

    // Get pointers to the texture resources inside the shader.
    m_fireTexturePtr = m_effect->GetVariableByName("fireTexture")->AsShaderResource();
    m_noiseTexturePtr = m_effect->GetVariableByName("noiseTexture")->AsShaderResource();
    m_alphaTexturePtr = m_effect->GetVariableByName("alphaTexture")->AsShaderResource();

    // Get the pointer to the frame time inside the shader.
    m_frameTimePtr = m_effect->GetVariableByName("frameTime")->AsScalar();

    // Get a pointer to the scrolling speeds array inside the shader.
    m_scrollSpeedsPtr = m_effect->GetVariableByName("scrollSpeeds")->AsVector();

    // Get a pointer to the noise scaling array inside the shader.
    m_scalesPtr = m_effect->GetVariableByName("scales")->AsVector();

    // Get the pointers to the three noise distortion arrays inside the shader.
    m_distortion1Ptr = m_effect->GetVariableByName("distortion1")->AsVector();
    m_distortion2Ptr = m_effect->GetVariableByName("distortion2")->AsVector();
    m_distortion3Ptr = m_effect->GetVariableByName("distortion3")->AsVector();

    // Get the pointers to the perturbation scale and bias variables inside the shader.
    m_distortionScalePtr = m_effect->GetVariableByName("distortionScale")->AsScalar();
    m_distortionBiasPtr = m_effect->GetVariableByName("distortionBias")->AsScalar();

    return true;
}


void FireShaderClass::ShutdownShader()
{
    // Release the perturbation scale and bias pointers.
    m_distortionScalePtr = 0;
    m_distortionBiasPtr = 0;

    // Release the pointers to the distortion arrays in the shader.
    m_distortion1Ptr = 0;
    m_distortion2Ptr = 0;
    m_distortion3Ptr = 0;

    // Release the pointer to the noise scaling array in the shader.
    m_scalesPtr = 0;

    // Release the pointer to the scroll speed array in the shader.
    m_scrollSpeedsPtr = 0;

    // Release the pointer to the frame time in the shader.
    m_frameTimePtr = 0;

    // Release the pointers to the textures in the shader.
    m_fireTexturePtr = 0;
    m_noiseTexturePtr = 0;
    m_alphaTexturePtr = 0;

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


void FireShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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


void FireShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
        ID3D10ShaderResourceView* fireTexture, ID3D10ShaderResourceView* noiseTexture,
        ID3D10ShaderResourceView* alphaTexture, float frameTime, D3DXVECTOR3 scrollSpeeds,
        D3DXVECTOR3 scales, D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2,
        D3DXVECTOR2 distortion3, float distortionScale, float distortionBias)
{
    // Set the world matrix variable inside the shader.
    m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);

    // Set the view matrix variable inside the shader.
    m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);

    // Set the projection matrix variable inside the shader.
    m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

    // Bind the textures.
    m_fireTexturePtr->SetResource(fireTexture);
    m_noiseTexturePtr->SetResource(noiseTexture);
    m_alphaTexturePtr->SetResource(alphaTexture);

    // Set the frame time in the shader.
    m_frameTimePtr->SetFloat(frameTime);

    // Set the scolling speed array for the three noise textures in the shader.
    m_scrollSpeedsPtr->SetFloatVector((float*)&scrollSpeeds);

    // Set the scaling array for the three noise textures in the shader.
    m_scalesPtr->SetFloatVector((float*)&scales);

    // Set the three distortion arrays for the three noise textures in the shader.
    m_distortion1Ptr->SetFloatVector((float*)&distortion1);
    m_distortion2Ptr->SetFloatVector((float*)&distortion2);
    m_distortion3Ptr->SetFloatVector((float*)&distortion3);

    // Set the perturbation scale and bias in the shader.
    m_distortionScalePtr->SetFloat(distortionScale);
    m_distortionBiasPtr->SetFloat(distortionBias);

    return;
}


void FireShaderClass::RenderShader(ID3D10Device* device, int indexCount)
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
