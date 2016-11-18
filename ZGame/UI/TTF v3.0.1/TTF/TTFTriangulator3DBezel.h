// *************************************************************************************
//	TTFTriangulator3DBezel.h
//
//		Triangulator that emits 3D glyphs without curvature data.
//		Curves are approximated by linear segments.
//		The function GetCurveDivideCount() must be overriden.
//		Takes 5 values to describe the bezel z1, z2, b0, b1, b2 (z0 is implicitly 0).
//		z0, z1, and z2 are the z values of the 3 'slices', front face, middle, and back face.
//		b0, b1, b2 are the 'bold' values of each slice.
//
//																																Ryan Boghean
//																																April 2015
//
// *************************************************************************************

# pragma once

# include <exception>
# include <vector>
# include <cstdint>
# include <algorithm>

# include "TTFExceptions.h"
# include "TTFMath.h"
# include "TTFTypes.h"
# include "TTFTriangulator2D.h"
# include "TTFTriangulator2DLinear.h"


// ---------------------------------------------------------------------------------------------------------------------------
//	TTFCore namespace
// ---------------------------------------------------------------------------------------------------------------------------

namespace TTFCore
{


// ---------------------------------------------------------------------------------------------------------------------------
//	Triangulator3DBezel
//		- extrude is the distance to extrude the 3d mesh
//		- extrude should probably be positive for a LH coordinate system, negative for a right
// ---------------------------------------------------------------------------------------------------------------------------

template <typename TVert, typename TTri> class Triangulator3DBezel : public Triangulator2DLinear<TVert,TTri>
{
    friend class Font;

private:
    // hide Triangulator2D functions
    TCoord GetBold() const;
    void SetBold(TCoord);

protected:
    // Triangulator3DBezel data
    TCoord z1, z2;									// z values of bezel extrude (z0 is implicitly 0)
    TCoord b0, b1, b2;							// bold values each extruded layer
    std::vector<TVert> temp_verts;		// temporary vertices

    // Triangulator3DBezel functions
    void BezelMesh();

    // Triangulator private interface
    std::vector<ContourPoint>& GetContours();		// returns a vector to the contour vector
    void AppendTriangulation();									// triangulates and appends any contour data into the mesh data

public:
    Triangulator3DBezel(TCoord z1, TCoord z2, TCoord b0, TCoord b1, TCoord b2);
    Triangulator3DBezel(TriangulatorFlags, TCoord z1, TCoord z2, TCoord b0, TCoord b1, TCoord b2);

    // bezel access
    void SetBezelValues(TCoord z1, TCoord z2, TCoord b0, TCoord b1, TCoord b2);
};


// ---------------------------------------------------------------------------------------------------------------------------
//	template code
// ---------------------------------------------------------------------------------------------------------------------------

// ----- constructor/destructor -----
template <typename TVert, typename TTri> Triangulator3DBezel<TVert, TTri>::Triangulator3DBezel(TCoord z1_, TCoord z2_, TCoord b0_, TCoord b1_, TCoord b2_) :
    Triangulator2DLinear<TVert,TTri>(TriangulatorFlags::none)
{
    z1 = z1_;
    z2 = z2_;
    b0 = b0_;
    b1 = b1_;
    b2 = b2_;
}

template <typename TVert, typename TTri> Triangulator3DBezel<TVert, TTri>::Triangulator3DBezel(TriangulatorFlags flags_, TCoord z1_, TCoord z2_, TCoord b0_, TCoord b1_, TCoord b2_) :
    Triangulator2DLinear<TVert,TTri>(flags_)
{
    z1 = z1_;
    z2 = z2_;
    b0 = b0_;
    b1 = b1_;
    b2 = b2_;
}


// ----- Triangulator 3D functions -----
template <typename TVert, typename TTri> void Triangulator3DBezel<TVert, TTri>::BezelMesh()
{

    temp_verts = verts;
    size_t n = verts.size();
    verts.reserve(n * 3);

    // construct back vertices
    bold_offset = b2;
    ApplyBold();
    for (size_t i = 0; i < n; ++i) verts[i].z = z2;

    // construct mid vertices
    verts.insert(verts.begin(), temp_verts.begin(), temp_verts.end());
    bold_offset = b1;
    ApplyBold();
    for (size_t i = 0; i < n; ++i) verts[i].z = z1;

    // construct front vertices
    verts.insert(verts.begin(), temp_verts.begin(), temp_verts.end());
    bold_offset = b0;
    ApplyBold();

    // construct back faces
    size_t m = tris.size();
    size_t n2 = n * 2;
    for (size_t i = 0; i < m; ++i)
    {
        TTri t = tris[i];
        tris.push_back(TTri(t.i0 + n2, t.i2 + n2, t.i1 + n2, 0));		// duplicate triangle and reverse winding order
    }

    // create edges
    for (Edge e : edges)
    {
        size_t v0 = e.i0;
        size_t v1 = e.i1;
        size_t v2 = e.i0 + n;
        size_t v3 = e.i1 + n;
        size_t v4 = e.i0 + n2;
        size_t v5 = e.i1 + n2;
        tris.push_back(TTri(v0,v2,v1,0));
        tris.push_back(TTri(v1,v2,v3,0));
        tris.push_back(TTri(v2,v4,v3,0));
        tris.push_back(TTri(v3,v4,v5,0));
    }

    // clean up
    bold_offset = 0;
    temp_verts.clear();
}


// ----- Triangulator private interface -----
template <typename TVert, typename TTri> std::vector<ContourPoint>& Triangulator3DBezel<TVert, TTri>::GetContours()
{
    return contours;
}

template <typename TVert, typename TTri> void Triangulator3DBezel<TVert, TTri>::AppendTriangulation()
{
    // Clear() has been called,  GetContours() called and the contour points filled

    TraceContour();
    TriangulateEdges((flags & TriangulatorFlags::use_cdt) == TriangulatorFlags::use_cdt);
    CreateTris();
    if (italic_offset_x != 0 && italic_offset_y != 0) ApplyItalic();
    BezelMesh();

    if ((flags & TriangulatorFlags::remove_unused_verts) == TriangulatorFlags::remove_unused_verts)
    {
        RemoveUnusedVerts();
    }

    // clear temporary data
    contours.clear();
    edges.clear();
    segs.clear();
    bounds.clear();
    tri_edges.clear();
}


// ----- bezel access -----
template <typename TVert, typename TTri> void Triangulator3DBezel<TVert, TTri>::SetBezelValues(TCoord z1, TCoord z2, TCoord b0, TCoord b1, TCoord b2)
{
    z1 = z1_;
    z2 = z2_;
    b0 = b0_;
    b1 = b1_;
    b2 = b2_;
}


// ---------------------------------------------------------------------------------------------------------------------------
//	End
// ---------------------------------------------------------------------------------------------------------------------------

}			// end of TTFCore namespace
