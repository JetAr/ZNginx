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
#include "PCH.h"
#include "Triangle3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Triangle3f::Triangle3f()
{
}
//--------------------------------------------------------------------------------
Triangle3f::Triangle3f( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 )
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
}
//--------------------------------------------------------------------------------
Triangle3f::~Triangle3f()
{
}
//--------------------------------------------------------------------------------
eSHAPE Triangle3f::GetShapeType( ) const
{
    return( TRIANGLE );
}
//--------------------------------------------------------------------------------
