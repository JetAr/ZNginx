// *************************************************************************************
//	TTFTypes.h
//
//		Miscellaneous types used in the TTF library.
//
//																																		Ryan Boghean
//																																		March 2015
//
// *************************************************************************************

# pragma once

# include <vector>
# include <cstdint>

# include "TTFExceptions.h"
# include "TTFMath.h"


// ---------------------------------------------------------------------------------------------------------------------------
//	TTFCore namespace
// ---------------------------------------------------------------------------------------------------------------------------

namespace TTFCore
{


// ---------------------------------------------------------------------------------------------------------------------------
//	ContourPoint
//		- a point on the line that delineates the contours of the glyph
//		- flags come straight from the font file
//		- pretty much only pos and OnCurve() means anything outside of a TTFFont object
//
//		implemented in: TTFTypes.cpp
// ---------------------------------------------------------------------------------------------------------------------------

struct ContourPoint
{
    vec2t pos;
    bool end_point;
    uint8_t flags;

    // helper functions
    ContourPoint();

    bool OnCurve() const;					// returns true if this point is on curve, false if off curve (control point)
    bool XShortVector() const;			// x coordinate is 1 byte
    bool YShortVector() const;			// y coordinate is 1 byte

    bool XIsSame() const;					// used when XShortVector() returns false
    bool XIsDifferent() const;				// used when XShortVector() returns false
    bool XIsPositive() const;				// used when XShortVector() returns true
    bool XIsNegative() const;				// used when XShortVector() returns true

    bool YIsSame() const;					// used when YShortVector() returns false
    bool YIsDifferent() const;				// used when YShortVector() returns false
    bool YIsPositive() const;				// used when YShortVector() returns true
    bool YIsNegative() const;				// used when YShortVector() returns true
};

typedef std::vector<ContourPoint> ContourData;
typedef std::vector<ContourPoint>::const_iterator CItr;


// ---------------------------------------------------------------------------------------------------------------------------
//	internal types used for trinangulation
//		- Edge's form a linked list, delineating the contours of the glyph, ContourPoint's are converted to Edge's
//		- a LineSegment is an inner segment or line that connects the vertices of two different edges
//		- LineSegment's are 'candidate' edges of the triangulation
//		- a Bound is an Edge or LineSegment that cannot be crossed (used to cull LineSegments)
//		- a TriEdge is a final triangle edge and used for final triangle construction
//		- TriEdge's are directed (ie. they have an implied direction from i0 -> i1)
//		- not all Edge's are TriEdge's or vice-versa, but they are similar
//
//		implemented in: TTFTypes.cpp
// ---------------------------------------------------------------------------------------------------------------------------

struct Edge
{
    size_t i0, i1;
    size_t pe, ne;							// previous edge offset, next edge offset (forms a linked list)
    Edge(size_t i0, size_t i1);
    Edge(size_t i0, size_t i1, size_t pe, size_t ne);
};

struct LineSegment
{
    size_t i0, i1;
    int32_t length, inscribe;
    LineSegment(size_t i0, size_t i1, int32_t length);
    LineSegment(size_t i0, size_t i1, int32_t length, int32_t inscribe);
};

struct Bound
{
    size_t i0, i1;
    Bound(size_t i0, size_t i1);
};

struct TriEdge
{
    size_t i0, i1;								// vertex indices
    bool in_use;								// start as true and become false when used in creating a tri
    TriEdge(size_t i0, size_t i1);
};


// ---------------------------------------------------------------------------------------------------------------------------
//	TriangulatorFlags types
//		- a few flags for various triangulators
//		- use_cdt enforces the use of a constrained delaunay triangulation
//
//		implemented in: TTFTypes.cpp
// ---------------------------------------------------------------------------------------------------------------------------

enum class TriangulatorFlags : uint8_t
{
    none = 0,							// no flags
    use_cdt = 1,						// use a constrained delaunay triangulation
    remove_unused_verts,		// remove any unused vertices
};

TriangulatorFlags operator~(TriangulatorFlags);
TriangulatorFlags operator&(TriangulatorFlags, TriangulatorFlags);
TriangulatorFlags operator|(TriangulatorFlags, TriangulatorFlags);
TriangulatorFlags operator^(TriangulatorFlags, TriangulatorFlags);
TriangulatorFlags& operator&=(TriangulatorFlags&, TriangulatorFlags);
TriangulatorFlags& operator|=(TriangulatorFlags&, TriangulatorFlags);
TriangulatorFlags& operator^=(TriangulatorFlags&, TriangulatorFlags);


// ---------------------------------------------------------------------------------------------------------------------------
//	End
// ---------------------------------------------------------------------------------------------------------------------------

}			// end of TTFCore namespace

