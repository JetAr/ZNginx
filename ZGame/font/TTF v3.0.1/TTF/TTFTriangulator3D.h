// *************************************************************************************
//	TTFTriangulator3D.h
//
//		Triangulator that emits 3D glyphs without curvature data.
//		Curves are approximated by linear segments.
//		The function GetCurveDivideCount() must be overriden.
//
//																																Ryan Boghean
//																																April 2012
//																																January 2014
//																																June 2014
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
//	Triangulator3D
//		- extrude is the distance to extrude the 3d mesh
//		- extrude should probably be positive for a LH coordinate system
// ---------------------------------------------------------------------------------------------------------------------------

template <typename TVert, typename TTri> class Triangulator3D : public Triangulator2DLinear<TVert,TTri>
{
    friend class Font;

protected:
    // Triangulator 3D data/functions
    TCoord extrude;
    void ExtrudeMesh();

    // Triangulator private interface
    std::vector<ContourPoint>& GetContours();		// returns a vector to the contour vector
    void AppendTriangulation();									// triangulates and appends any contour data into the mesh data

public:
    Triangulator3D(TCoord extrude);
    Triangulator3D(TriangulatorFlags, TCoord extrude);

    // extrude access
    TCoord GetExtrude() const;
    void SetExtrude(TCoord);
};


// ---------------------------------------------------------------------------------------------------------------------------
//	template code
// ---------------------------------------------------------------------------------------------------------------------------

// ----- constructor/destructor -----
template <typename TVert, typename TTri> Triangulator3D<TVert, TTri>::Triangulator3D(TCoord extrude_) :
    Triangulator2DLinear<TVert,TTri>(TriangulatorFlags::none)
{
    extrude = extrude_;
}

template <typename TVert, typename TTri> Triangulator3D<TVert, TTri>::Triangulator3D(TriangulatorFlags flags_, TCoord extrude_) :
    Triangulator2DLinear<TVert,TTri>(flags_)
{
    extrude = extrude_;
}


// ----- Triangulator 3D functions -----
template <typename TVert, typename TTri> void Triangulator3D<TVert, TTri>::ExtrudeMesh()
{

    // copy and duplicate vertices
    size_t n = verts.size();
    verts.reserve(n * 2);
    for (size_t i = 0; i < n; ++i)
    {
        TVert v = verts[i];
        verts.push_back(TVert(v.x, v.y, extrude));
    }

    // duplicate tris
    size_t m = tris.size();
    for (size_t i = 0; i < m; ++i)
    {
        TTri t = tris[i];
        tris.push_back(TTri(t.i0 + n, t.i2 + n, t.i1 + n, 0));		// duplicate triangle and reverse winding order
    }

    // create edges
    for (auto i = edges.begin(); i != edges.end(); ++i)
    {
        size_t v0 = i->i0;
        size_t v1 = i->i1;
        size_t v2 = i->i0 + n;
        size_t v3 = i->i1 + n;
        tris.push_back(TTri(v0,v2,v1,0));
        tris.push_back(TTri(v1,v2,v3,0));
    }
}


// ----- Triangulator private interface -----
template <typename TVert, typename TTri> std::vector<ContourPoint>& Triangulator3D<TVert, TTri>::GetContours()
{
    return contours;
}

template <typename TVert, typename TTri> void Triangulator3D<TVert, TTri>::AppendTriangulation()
{
    // Clear() has been called,  GetContours() called and the contour points filled

    TraceContour();
    TriangulateEdges((flags & TriangulatorFlags::use_cdt) == TriangulatorFlags::use_cdt);
    CreateTris();
    if (bold_offset != 0) ApplyBold();
    if (italic_offset_x != 0 && italic_offset_y != 0) ApplyItalic();

    ExtrudeMesh();
    if ((flags & TriangulatorFlags::remove_unused_verts) == TriangulatorFlags::remove_unused_verts)
    {
        RemoveUnusedVerts();
    }

    // clear temporary data (if the triangulator is copied, this does not need to be copied with it)
    contours.clear();
    edges.clear();
    segs.clear();
    bounds.clear();
    tri_edges.clear();
}


// ----- extrude access -----
template <typename TVert, typename TTri> typename Triangulator3D<TVert, TTri>::TCoord Triangulator3D<TVert, TTri>::GetExtrude() const
{
    return extrude;
}

template <typename TVert, typename TTri> void Triangulator3D<TVert, TTri>::SetExtrude(TCoord extrude_)
{
    extrude = extrude_;
}


// ---------------------------------------------------------------------------------------------------------------------------
//	End
// ---------------------------------------------------------------------------------------------------------------------------

}			// end of TTFCore namespace

