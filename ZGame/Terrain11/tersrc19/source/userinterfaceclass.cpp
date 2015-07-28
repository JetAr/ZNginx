///////////////////////////////////////////////////////////////////////////////
// Filename: userinterfaceclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "userinterfaceclass.h"


UserInterfaceClass::UserInterfaceClass()
{
    m_Font1 = 0;
    m_FpsString = 0;
    m_PositionStrings = 0;
    m_VideoStrings = 0;
}


UserInterfaceClass::UserInterfaceClass(const UserInterfaceClass& other)
{
}


UserInterfaceClass::~UserInterfaceClass()
{
}


bool UserInterfaceClass::Initialize(D3DClass* Direct3D, int screenWidth, int screenHeight)
{
    bool result;
    int i;
    char videoCard[128];
    char videoString[150];
    int memory;
    char videoMemory[64];
    char tempString[16];


    // Create the first font object.
    m_Font1 = new FontClass;
    if(!m_Font1)
    {
        return false;
    }

    // Initialize the first font object.
    result = m_Font1->Initialize(Direct3D->GetDevice(), "../Engine/data/font/font01.txt", L"../Engine/data/font/font01.dds", 32.0f, 3);
    if(!result)
    {
        return false;
    }

    // Create the text object for the fps string.
    m_FpsString = new TextClass;
    if(!m_FpsString)
    {
        return false;
    }

    // Initialize the fps text string.
    result = m_FpsString->Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Initial the previous frame fps.
    m_previousFps = -1;

    // Create the text objects for the position strings.
    m_PositionStrings = new TextClass[6];
    if(!m_PositionStrings)
    {
        return false;
    }

    // Initialize the position text strings.
    result = m_PositionStrings[0].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "X: 0",  10, 90,  1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_PositionStrings[1].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "Y: 0",  10, 110,  1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_PositionStrings[2].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "Z: 0",  10, 130,  1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_PositionStrings[3].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "rX: 0", 10, 170, 1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_PositionStrings[4].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "rY: 0", 10, 190, 1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_PositionStrings[5].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 16, "rZ: 0", 10, 210, 1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Initialize the previous frame position.
    for(i=0; i<6; i++)
    {
        m_previousPosition[i] = -1;
    }

    // Setup the video card strings.
    Direct3D->GetVideoCardInfo(videoCard, memory);
    strcpy_s(videoString, "Video Card: ");
    strcat_s(videoString, videoCard);

    _itoa_s(memory, tempString, 10);
    strcpy_s(videoMemory, "Video Memory: ");
    strcat_s(videoMemory, tempString);
    strcat_s(videoMemory, " MB");

    m_VideoStrings = new TextClass[2];
    if(!m_VideoStrings)
    {
        return false;
    }

    // Initialize the position text strings.
    result = m_VideoStrings[0].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 256, videoString,  10, 10,  1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }
    result = m_VideoStrings[1].Initialize(Direct3D->GetDevice(), m_Font1, screenWidth, screenHeight, 64, videoMemory,  10, 30,  1.0f, 1.0f, 1.0f, false, Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    return true;
}


void UserInterfaceClass::Shutdown()
{
    // Release the position text strings.
    if(m_VideoStrings)
    {
        m_VideoStrings[0].Shutdown();
        m_VideoStrings[1].Shutdown();

        delete [] m_VideoStrings;
        m_VideoStrings = 0;
    }

    // Release the position text strings.
    if(m_PositionStrings)
    {
        m_PositionStrings[0].Shutdown();
        m_PositionStrings[1].Shutdown();
        m_PositionStrings[2].Shutdown();
        m_PositionStrings[3].Shutdown();
        m_PositionStrings[4].Shutdown();
        m_PositionStrings[5].Shutdown();

        delete [] m_PositionStrings;
        m_PositionStrings = 0;
    }

    // Release the fps text string.
    if(m_FpsString)
    {
        m_FpsString->Shutdown();
        delete m_FpsString;
        m_FpsString = 0;
    }

    // Release the font object.
    if(m_Font1)
    {
        m_Font1->Shutdown();
        delete m_Font1;
        m_Font1 = 0;
    }

    return;
}


bool UserInterfaceClass::Frame(int fps, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, ID3D11DeviceContext* deviceContext)
{
    bool result;


    // Update the fps string.
    result = UpdateFpsString(fps, deviceContext);
    if(!result)
    {
        return false;
    }

    // Update the position strings.
    result = UpdatePositionStrings(posX, posY, posZ, rotX, rotY, rotZ, deviceContext);
    if(!result)
    {
        return false;
    }

    return true;
}


void UserInterfaceClass::Render(D3DClass* Direct3D, ShaderManagerClass* ShaderManager, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX orthoMatrix)
{
    int i;


    // Turn off the Z buffer and enable alpha blending to begin 2D rendering.
    Direct3D->TurnZBufferOff();
    Direct3D->TurnOnAlphaBlending();

    // Render the fps string.
    m_FpsString->Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());

    // Render the position and rotation strings.
    for(i=0; i<6; i++)
    {
        m_PositionStrings[i].Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());
    }

    // Render the video card strings.
    for(i=0; i<2; i++)
    {
        m_VideoStrings[i].Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());
    }

    // Turn the Z buffer back on and disable alpha blending now that the 2D rendering has completed.
    Direct3D->TurnZBufferOn();
    Direct3D->TurnOffAlphaBlending();

    return;
}


bool UserInterfaceClass::UpdateFpsString(int fps, ID3D11DeviceContext* deviceContext)
{
    char tempString[16];
    char finalString[16];
    float red, green, blue;
    bool result;


    // Check if the fps from the previous frame was the same, if so don't need to update the text string.
    if(m_previousFps == fps)
    {
        return true;
    }

    // Store the fps for checking next frame.
    m_previousFps = fps;

    // Truncate the fps to below 100,000.
    if(fps > 99999)
    {
        fps = 99999;
    }

    // Convert the fps integer to string format.
    _itoa_s(fps, tempString, 10);

    // Setup the fps string.
    strcpy_s(finalString, "Fps: ");
    strcat_s(finalString, tempString);

    // If fps is 60 or above set the fps color to green.
    if(fps >= 60)
    {
        red = 0.0f;
        green = 1.0f;
        blue = 0.0f;
    }

    // If fps is below 60 set the fps color to yellow.
    if(fps < 60)
    {
        red = 1.0f;
        green = 1.0f;
        blue = 0.0f;
    }

    // If fps is below 30 set the fps color to red.
    if(fps < 30)
    {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    // Update the sentence vertex buffer with the new string information.
    result = m_FpsString->UpdateSentence(m_Font1, finalString, 10, 50, red, green, blue, deviceContext);
    if(!result)
    {
        return false;
    }

    return true;
}


bool UserInterfaceClass::UpdatePositionStrings(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, ID3D11DeviceContext* deviceContext)
{
    int positionX, positionY, positionZ, rotationX, rotationY, rotationZ;
    char tempString[16];
    char finalString[16];
    bool result;


    // Convert the float values to integers.
    positionX = (int)posX;
    positionY = (int)posY;
    positionZ = (int)posZ;
    rotationX = (int)rotX;
    rotationY = (int)rotY;
    rotationZ = (int)rotZ;

    // Update the position strings if the value has changed since the last frame.
    if(positionX != m_previousPosition[0])
    {
        m_previousPosition[0] = positionX;
        _itoa_s(positionX, tempString, 10);
        strcpy_s(finalString, "X: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[0].UpdateSentence(m_Font1, finalString, 10, 90, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    if(positionY != m_previousPosition[1])
    {
        m_previousPosition[1] = positionY;
        _itoa_s(positionY, tempString, 10);
        strcpy_s(finalString, "Y: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[1].UpdateSentence(m_Font1, finalString, 10, 110, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    if(positionZ != m_previousPosition[2])
    {
        m_previousPosition[2] = positionZ;
        _itoa_s(positionZ, tempString, 10);
        strcpy_s(finalString, "Z: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[2].UpdateSentence(m_Font1, finalString, 10, 130, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    if(rotationX != m_previousPosition[3])
    {
        m_previousPosition[3] = rotationX;
        _itoa_s(rotationX, tempString, 10);
        strcpy_s(finalString, "rX: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[3].UpdateSentence(m_Font1, finalString, 10, 170, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    if(rotationY != m_previousPosition[4])
    {
        m_previousPosition[4] = rotationY;
        _itoa_s(rotationY, tempString, 10);
        strcpy_s(finalString, "rY: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[4].UpdateSentence(m_Font1, finalString, 10, 190, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    if(rotationZ != m_previousPosition[5])
    {
        m_previousPosition[5] = rotationZ;
        _itoa_s(rotationZ, tempString, 10);
        strcpy_s(finalString, "rZ: ");
        strcat_s(finalString, tempString);
        result = m_PositionStrings[5].UpdateSentence(m_Font1, finalString, 10, 210, 1.0f, 1.0f, 1.0f, deviceContext);
        if(!result)
        {
            return false;
        }
    }

    return true;
}
