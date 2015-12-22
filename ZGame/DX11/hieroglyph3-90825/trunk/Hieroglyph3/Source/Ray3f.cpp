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
#include "Ray3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Ray3f::Ray3f()
{
}
//--------------------------------------------------------------------------------
Ray3f::Ray3f( const Vector3f& position, const Vector3f& direction )
{
    Origin = position;
    Direction = direction;
}
//--------------------------------------------------------------------------------
Ray3f::~Ray3f()
{
}
//--------------------------------------------------------------------------------
eSHAPE Ray3f::GetShapeType() const
{
    return( RAY );
}
//--------------------------------------------------------------------------------
