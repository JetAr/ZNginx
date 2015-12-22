﻿//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// CompositeShape
//
//--------------------------------------------------------------------------------
#ifndef CompositeShape_h
#define CompositeShape_h
//--------------------------------------------------------------------------------
#include "Shape3D.h"
#include "Ray3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
class CompositeShape
{
public:
    CompositeShape( );
    virtual ~CompositeShape( );

    CompositeShape* DeepCopy( );

    void AddShape( Shape3D* pShape );
    bool RayIntersection( const Ray3f& ray, float* fDist );

    int GetNumberOfShapes() const;

    std::vector< Shape3D* > m_Shapes;
};
};
//--------------------------------------------------------------------------------
#endif // CompositeShape_h
