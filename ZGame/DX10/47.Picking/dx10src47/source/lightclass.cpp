////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDirection(float x, float y, float z)
{
    m_direction = D3DXVECTOR3(x, y, z);
    return;
}


D3DXVECTOR3 LightClass::GetDirection()
{
    return m_direction;
}
