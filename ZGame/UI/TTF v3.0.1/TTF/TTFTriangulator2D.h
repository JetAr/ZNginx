// *************************************************************************************
//	TTFTriangulator2D.h
//
//		Triangulator that emits 2D glyphs with curvature data.
//		Glyphs are output'd as an array of either TriSmall's or TriLarge's.
//
//																																Ryan Boghean
//																																April 2012
//																																January 2014
//																																June 2014
//																																March 2015
//																																October 2016
//
// *************************************************************************************

# pragma once

# include <exception>
# include <vector>
# include <cstdint>
# include <algorithm>
# include <type_traits>
# include <limits>

# include "TTFExceptions.h"
# include "TTFMath.h"
# include "TTFTypes.h"


// ---------------------------------------------------------------------------------------------------------------------------
//	TTFCore namespace
// ---------------------------------------------------------------------------------------------------------------------------

namespace TTFCore
{


// ---------------------------------------------------------------------------------------------------------------------------
//	TriSmall
//		- a small triangle type used for a compact 2d mesh representation
//		- valid for glyph vertex counts up to 1024 vertices
//		- IsValid() in Triangulator2DI can be used to test for a valid triagulation
//
//	TriLarge
//		- a larger triangle type used for a compact 2d mesh representation
//		- valid for glyph vertex counts up to 64k vertices
//		- IsValid() in Triangulator2DI can be used to test for a valid triagulation
//
//		implemented in: TTFTriangulator2D.cpp
// ---------------------------------------------------------------------------------------------------------------------------

// ----- TriSmall -----
struct TriSmall
{
    uint32_t i0 : 10;				// implied tex coords (0.0f, 0.0f)
    uint32_t i1 : 10;				// implied tex coords (0.5f, 0.0f)
    uint32_t i2 : 10;				// implied tex coords (1.0f, 1.0f)
    int32_t coef : 2;				// shader coefficient (-1 = inner edge / CCW, 0 = solid/non-edge tri, 1 = outter edge / CW)
    TriSmall(size_t i0, size_t i1, size_t i2, int32_t coef);
};
static_assert(sizeof(TriSmall) == 4, "TriSmall bitfield is incorrect.  Perhaps a non-MS compiler?");


// ----- TriLarge -----
struct TriLarge
{
    uint16_t i0;				// implied tex coords (0.0f, 0.0f)
    uint16_t i1;				// implied tex coords (0.5f, 0.0f)
    uint16_t i2;				// implied tex coords (1.0f, 1.0f)
    int16_t coef;				// shader coefficient (-1 = inner edge / CCW, 0 = solid/non-edge tri, 1 = outter edge / CW)
    TriLarge(size_t i0, size_t i1, size_t i2, int16_t coef);
};
static_assert(sizeof(TriLarge) == 8, "TriLarge bitfield is incorrect.  Perhaps a non-MS compiler?");


// ---------------------------------------------------------------------------------------------------------------------------
//	Triangulator2D
//		- basic glyph triangulator
//		- emits 2d glyphs with curvature data (ie. uses a pixel shader to render curves)
//		- bold offset is number of 'font units' to offset vertices along the 'normal'
//		- italic takes a two values, (1,1) for example would sheer at a 45 degree angle
// ---------------------------------------------------------------------------------------------------------------------------

template <typename TVert, typename TTri> class Triangulator2D
{
    friend class Font;

protected:
    // internal types
    typedef decltype(TVert::x) TCoord;
    static const size_t index_error = std::numeric_limits<size_t>::max();

    // triangulation data/caches
    std::vector<ContourPoint> contours;		// contour points
    std::vector<Edge> edges;						// edges prior to triangulation
    std::vector<LineSegment> segs;				// inner segment candidates
    std::vector<Bound> bounds;					// bounds used for checking edge/segment intersections
    std::vector<TriEdge> tri_edges;				// final valid triangle edges
    std::vector<TVert> verts;							// vertices
    std::vector<TVert> bverts;						// copy of vertices used for ApplyBold function
    std::vector<TTri> tris;								// final triangles

    // style data
    TriangulatorFlags flags;												// triangulation flags
    TCoord bold_offset, italic_offset_x, italic_offset_y;		// bold/italics

    // internal helpers
    size_t AddVertex(size_t start, TVert);			// adds a vertex to verts, returns its index
    size_t AddEdge(size_t i0, size_t i1);				// adds an edge to edges, does not add if the edge is degenerate, returns index (or index_error)
    size_t FindEdge(size_t i0, size_t i1);				// returns the index of the edge to the corresponding indices (or index_error if none was found)

    // TraceContour
    void TraceContourHelper(ContourPoint cp, TVert& pp, TVert& ip, bool& ppc, size_t vbo);
    void TraceContour(CItr begin, CItr end);		// creates edge data from contours
    void TraceContour();										// iterates over contour points, creates initial edges/verts and 'curve tris'

    // SubdivideCurves
    size_t IntersectTri(size_t i0, size_t i1) const;						// returns the triangle index that intersects with the given line (p0 -> p1)
    bool SubdivideTri(size_t tri_index);									// subdivides the triangle, returns true for a valid subdivide, false for an error (no more subdivisions can occur)
    bool SubdivideTris(size_t tri_index0, size_t tri_index1);		// subdivides the large of the two triangles, returns true for a valid subdivide, false for an error (no more subdivisions can occur)
    bool SubdivideCurveHelper();												// checks for intersecting tris and subdivides if necessary, returns true if there are more tri's to subdivide, false otherwise
    void SubdivideCurves();														// check curve triangles for intersection, and subdivides if necessary

    // TriangulateEdges
    bool BoundIntersection(TVert p0, TVert p1) const;			// returns true if the segment (p0 -> p1) intersects with any bounds
    bool IdenticalSegment(size_t i0, size_t i1) const;			// returns true if the segment (p0 -> p1) is indentical to any other segment
    int32_t GetInscribeCount(TVert p0, TVert p1) const;		// returns the number of vertices that are inscribed by the circumcircle of the segment p0 -> p1
    void TriangulateEdges(bool use_cdt);								// adds triangulation line segments, creates tri_edges

    // CreateTris
    void ConstructTri(std::vector<TriEdge>::iterator);		// helper function for CreateTris
    void CreateTris();															// creates tris from tri_edges

    // RemoveUnusedVerts
    void SwapTriVertex(size_t i0, size_t i1);						// swaps i0 for i1 in any triangle
    size_t VertexUseCount(size_t vertex_index) const;		// returns the number of times a vertex is used in a tri
    bool VertexInUse(size_t vertex_index) const;				// returns if a vertex is in use
    void RemoveUnusedVerts();											// removes any unused vertices

    // modifier functions
    void ApplyBold();
    void ApplyItalic();

    // debug functions
    size_t VerifyEdgeList();														// ensure edges form a proper linked list (returns the index of the 1st edge that is invalid, or index_error on success)
    void CreateTrisDebug();													// converts tri_edges to lines
    void CreateTrisDebug2(int16_t line_width);						// converts edges to arrows
    void CreateTrisDebug3(int16_t line_width);						// converts segments to arrows
    void CreateTrisDebug4(int16_t width);								// converts verts to triangles
    void CreateTrisDebug5(int16_t line_width);						// converts bounds to arrows
    void CreateTrisDebug6(int16_t line_width, bool in_use);		// converts tri_edges to arrows which match the 'in_use' parameter

    // Triangulator interface
    std::vector<ContourPoint>& GetContours();		// returns a vector to the contour vector
    void AppendTriangulation();									// triangulates and appends any contour data into the mesh data

public:
    Triangulator2D();
    Triangulator2D(TriangulatorFlags);

    // public types
    typedef TVert VertexType;
    typedef TTri TriType;

    // Triangulator interface
    void Clear();			// clears current mesh data
    void Trim();			// clears and trims any allocated data

    // mesh access
    size_t GetVertexCount() const;
    size_t GetTriangleCount() const;
    TVert GetVertex(size_t) const;
    TTri GetTriangle(size_t) const;
    const TVert* GetVertexRaw() const;				// as an array of vertices
    const TTri* GetTriangleRaw() const;				// as an array of triangles
    TVert operator[](size_t) const;						// quick access to vertices
    const TTri* begin() const;								// allows easy iterating overtriangles, use operator[] to quickly get vertex
    const TTri* end() const;
    bool Isvalid() const;										// returns true if the current triangulation is valid (GetVertexCount() <= 1024 )

    // style access
    TriangulatorFlags GetFlags() const;
    void SetFlags(TriangulatorFlags);
    TCoord GetBold() const;
    void SetBold(TCoord);
    TCoord GetItalicX() const;
    TCoord GetItalicY() const;
    void SetItalic(TCoord, TCoord);
};


// ---------------------------------------------------------------------------------------------------------------------------
//	template code
// ---------------------------------------------------------------------------------------------------------------------------

// ----- constructor/destructor -----
template <typename TVert, typename TTri> Triangulator2D<TVert, TTri>::Triangulator2D()
{
    flags = TriangulatorFlags::none;
    bold_offset = 0;
    italic_offset_x = 0;
    italic_offset_y = 0;
}

template <typename TVert, typename TTri> Triangulator2D<TVert, TTri>::Triangulator2D(TriangulatorFlags flags_)
{
    flags = flags_;
    bold_offset = 0;
    italic_offset_x = 0;
    italic_offset_y = 0;
}


// ----- internal helpers -----
template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::AddVertex(size_t start, TVert v)
{
    for (size_t i = start; i < verts.size(); ++i)
    {
        if (verts[i] == v) return i;
    }
    size_t r = verts.size();
    verts.push_back(v);
    return r;
}

template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::AddEdge(size_t i0, size_t i1)
{
    if (i0 == i1) return index_error;
    size_t r = edges.size();
    edges.emplace_back(i0, i1);
    return r;
}

template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::FindEdge(size_t i0, size_t i1)
{
    for (size_t i = 0; i < edges.size(); ++i)
    {
        if (edges[i].i0 == i0 && edges[i].i1 == i1) return i;
    }
    return index_error;
}


// ----- TraceContour -----
template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::TraceContourHelper(ContourPoint cp, TVert& p0, TVert& p1, bool& ppc, size_t vbo)
{
    // p0, p1, p2 form the triangle vertices, with i0, i1, i2 being the corresponding indices
    // p0 is the previous on curve point
    // p1 is the off curve point (or ignored if no curve)
    // p2 is the current point under consideration (may be on or off curve depending)
    // when p2 is off curve a new point (pn) is constructed 1/2 way between p1 and p2
    // pn is on curve due to its construction

    // ppc is true when the previously processed point was on curve
    // cpc is true when the current processed point is on curve

    // intialize point variables
    TVert p2 = cp.pos;					// point current/under consideration
    bool cpc = cp.OnCurve();			// current point on curve

    // determine action
    if (ppc && cpc)
    {
        size_t i0 = AddVertex(vbo, p0);
        size_t i2 = AddVertex(vbo, p2);
        AddEdge(i0, i2);
        p0 = p2;
    }
    else if (ppc && !cpc)
    {
        p1 = p2;
        ppc = false;
    }
    else if (!ppc && cpc)
    {

        size_t i0 = AddVertex(vbo, p0);
        size_t i1 = AddVertex(vbo, p1);
        size_t i2 = AddVertex(vbo, p2);

        int32_t sign = GetTriSign(p0, p1, p2);
        if (sign > 0)
        {
            AddEdge(i0, i2);								// tri is an outer, need 1 edge (cap)
            tris.emplace_back(i0, i1, i2, +1);
        }
        else if (sign < 0)
        {
            AddEdge(i0, i1);								// tri is an inner, need both edges (wedge)
            AddEdge(i1, i2);
            tris.emplace_back(i0, i1, i2, -1);
        }
        else
        {
            AddEdge(i0, i1);								// control point sits right on the curve
            AddEdge(i1, i2);
        }

        p0 = p2;
        ppc = true;
    }
    else if (!ppc && !cpc)
    {

        TVert pn = MidPoint(p1, p2);			// new on curve point

        size_t i0 = AddVertex(vbo, p0);
        size_t i1 = AddVertex(vbo, p1);
        size_t i2 = AddVertex(vbo, pn);

        int32_t sign = GetTriSign(p0, p1, pn);
        if (sign > 0)
        {
            AddEdge(i0, i2);						// tri is an outer, need 1 edge (cap)
            tris.emplace_back(i0, i1, i2, +1);
        }
        else if (sign < 0)
        {
            AddEdge(i0, i1);						// tri is an inner, need both edges (wedge)
            AddEdge(i1, i2);
            tris.emplace_back(i0, i1, i2, -1);
        }
        else
        {
            AddEdge(i0, i1);						// control point sits right on the curve
            AddEdge(i1, i2);
        }

        p0 = pn;
        p1 = p2;
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::TraceContour(CItr begin, CItr end)
{

    // sanity checks
    if (end - begin < 2) return;

    // init variables
    size_t vbo = verts.size();				// store offset of 1st vertex
    size_t ebo = edges.size();				// store offset of 1st edge

    // trace contour
    if (begin->OnCurve())
    {
        TVert p0 = begin->pos;					// previous point on curve
        TVert p1;										// intermediate (off curve) point/control point
        bool ppc = true;							// previous point was on the curve

        for (auto i = begin + 1; i != end; ++i) TraceContourHelper(*i, p0, p1, ppc, vbo);
        TraceContourHelper(*end, p0, p1, ppc, vbo);
        TraceContourHelper(*begin, p0, p1, ppc, vbo);
    }
    else if ( (begin + 1)->OnCurve() )
    {
        TVert p0 = (begin + 1)->pos;
        TVert p1;
        bool ppc = true;

        for (auto i = begin + 2; i != end; ++i) TraceContourHelper(*i, p0, p1, ppc, vbo);
        TraceContourHelper(*end, p0, p1, ppc, vbo);
        TraceContourHelper(*begin, p0, p1, ppc, vbo);
        TraceContourHelper(*(begin + 1), p0, p1, ppc, vbo);
    }
    else
    {
        // neither begin nor begin + 1 is OnCurve
        // we have to create a new ContourPoint to start the contour
        ContourPoint cp;
        cp.end_point = false;
        cp.flags = 1;					// on curve
        cp.pos = MidPoint(begin->pos, (begin + 1)->pos);

        TVert p0 = cp.pos;
        TVert p1;
        bool ppc = true;

        for (auto i = begin + 1; i != end; ++i) TraceContourHelper(*i, p0, p1, ppc, vbo);
        TraceContourHelper(*end, p0, p1, ppc, vbo);
        TraceContourHelper(*begin, p0, p1, ppc, vbo);
        TraceContourHelper(cp, p0, p1, ppc, vbo);
    }

    // create edge linked list
    size_t eeo = edges.size() - 1;
    edges[ebo].pe = eeo;
    edges[ebo].ne = ebo + 1;
    for (size_t i = ebo + 1; i < eeo; ++i)
    {
        edges[i].pe = i - 1;
        edges[i].ne = i + 1;
    }
    edges[eeo].pe = eeo - 1;
    edges[eeo].ne = ebo;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::TraceContour()
{

    // separate the contour points into separate contours
    CItr begin = contours.begin();
    CItr end = contours.end();

    if (end - begin <= 1) return;
    CItr contour_begin = begin;
    for (auto i = begin; i != end; ++i)
    {
        if (i->end_point)
        {
            TraceContour(contour_begin, i);		// traces an individual contour
            contour_begin = i + 1;
        }
    }
}


// ----- SubdivideCurves -----
template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::IntersectTri(size_t i0, size_t i1) const
{

    TVert p0 = verts[i0];
    TVert p1 = verts[i1];

    for (size_t i = 0; i < tris.size(); ++i)
    {

        TTri t = tris[i];

        if (t.coef == -1 &&
                i0 != t.i0 && i1 != t.i2 &&
                (Intersect2D(p0, p1, verts[t.i0], verts[t.i1]) || Intersect2D(p0, p1, verts[t.i1], verts[t.i2]))
           )
        {
            return i;
        }

        // coef == 0 is deliberately ignored

        if (t.coef == 1 &&
                i0 != t.i0 && i1 != t.i1 &&
                Intersect2D(p0, p1, verts[t.i0], verts[t.i2])
           )
        {
            return i;
        }
    }

    return index_error;
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::SubdivideTri(size_t tri_index)
{

    TTri& t = tris[tri_index];

    // get tri indices/verts
    size_t i0 = t.i0;
    size_t i1 = t.i1;
    size_t i2 = t.i2;
    TVert p0 = verts[i0];
    TVert p1 = verts[i1];
    TVert p2 = verts[i2];

    // create new vertices
    TVert p3 = MidPoint(p0, p1);
    TVert p4 = MidPoint(p1, p2);
    TVert p5 = MidPoint(p3, p4);

    // sanity check
    if (p3 == p0 || p3 == p1) return false;
    if (p4 == p1 || p4 == p2) return false;
    if (p5 == p3 || p5 == p4) return false;

    // add new verts, create new indices
    size_t i3 = verts.size();
    verts.push_back(p3);			// AddVertex() isn't necessary here
    size_t i4 = verts.size();
    verts.push_back(p4);
    size_t i5 = verts.size();
    verts.push_back(p5);

    // create new triangle/edges
    if (t.coef == -1)
    {

        // create new triangles
        t = TTri(i0, i3, i5, -1);
        tris.emplace_back(i5, i4, i2, -1);

        // get edge indices/edges
        size_t es = edges.size();
        size_t ei0 = FindEdge(i0, i1);
        size_t ei1 = FindEdge(i1, i2);
        if (ei0 == index_error || ei1 == index_error) throw InternalError("Error in SubdivideTri(), invalid edge list.");
        Edge& e0 = edges[ei0];
        Edge& e1 = edges[ei1];
        if (e0.ne != ei1 || e1.pe != ei0) throw InternalError("Error in SubdivideTri(), invalid edge list.");

        // create new edges
        Edge ne0 = Edge(i0, i3, e0.pe, es);
        Edge ne1 = Edge(i3, i5, ei0, es + 1);
        Edge ne2 = Edge(i5, i4, es, ei1);
        Edge ne3 = Edge(i4, i2, es + 1, e1.ne);

        // add edges to list
        e0 = ne0;
        e1 = ne3;
        edges.push_back(ne1);
        edges.push_back(ne2);

        // done
        return true;
    }

    if (t.coef == 1)
    {

        // create new triangles
        t = TTri(i0, i3, i5, 1);
        tris.emplace_back(i5, i4, i2, 1);

        // get edge
        size_t es = edges.size();
        size_t ei = FindEdge(i0, i2);
        if (ei == index_error) throw InternalError("Error in SubdivideTri(), invalid edge list.");
        Edge& e = edges[ei];
        Edge& ne = edges[e.ne];

        size_t pei = e.pe;
        size_t nei = e.ne;

        // create new edges
        Edge ne0 = Edge(i0, i5, e.pe, es);			// ei
        Edge ne1 = Edge(i5, i2, ei, e.ne);			// es

        e = ne0;
        ne.pe = es;
        edges.push_back(ne1);

        // done
        return true;
    }

    // error, invalid coefficient
    throw InternalError("Error in SubdivideTri(), invalid coefficiant.");
    return false;
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::SubdivideTris(size_t tri_index0, size_t tri_index1)
{

    // approximate area of triangle 0
    TTri t0 = tris[tri_index0];
    TVert p0 = verts[t0.i0];
    TVert p1 = verts[t0.i1];
    TVert p2 = verts[t0.i2];
    auto area0 = std::abs(cross(p1 - p0, p2 - p0));

    // approximate area of triangle 1
    TTri t1 = tris[tri_index1];
    TVert q0 = verts[t1.i0];
    TVert q1 = verts[t1.i1];
    TVert q2 = verts[t1.i2];
    auto area1 = std::abs(cross(q1 - q0, q2 - q0));

    // subdivide larger tri
    return (area0 >= area1) ? SubdivideTri(tri_index0) : SubdivideTri(tri_index1);
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::SubdivideCurveHelper()
{
    for (size_t i = 0; i < tris.size(); ++i)
    {
        TTri t = tris[i];
        if (t.coef == 1)
        {
            size_t j = IntersectTri(t.i0, t.i2);
            if (j != index_error && SubdivideTris(i, j)) return true;			// don't return on false, just keep checking
        }
        if (t.coef == -1)
        {
            size_t j = IntersectTri(t.i0, t.i1);
            if (j != index_error && SubdivideTris(i, j)) return true;
            size_t k = IntersectTri(t.i1, t.i2);
            if (k != index_error && SubdivideTris(i, k)) return true;
        }
    }
    return false;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::SubdivideCurves()
{
    while (SubdivideCurveHelper()) {}
}


// ----- TriangulateEdges -----
template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::BoundIntersection(TVert p0, TVert p1) const
{
    for (Bound b : bounds)
    {
        if (Intersect2D(p0, p1, verts[b.i0], verts[b.i1])) return true;
    }
    return false;
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::IdenticalSegment(size_t i0, size_t i1) const
{
    for (LineSegment s : segs)
    {
        if ((s.i0 == i0 && s.i1 == i1) || (s.i0 == i1 && s.i1 == i0)) return true;
    }
    return false;
}

template <typename TVert, typename TTri> int32_t Triangulator2D<TVert, TTri>::GetInscribeCount(TVert p0, TVert p1) const
{
    // returns the number of vertices that are inscribed by the circumcircle of the segment p0 -> p1
    // used for an approximate CDT
    // requires 64-bit math (or floating point math, but I wanted to keep things as integers)

    // convenience variables
    int64_t p0x = static_cast<int64_t>(p0.x);
    int64_t p0y = static_cast<int64_t>(p0.y);
    int64_t p1x = static_cast<int64_t>(p1.x);
    int64_t p1y = static_cast<int64_t>(p1.y);

    // calculate 'radius'
    int64_t rtx = p0x - p1x;
    int64_t rty = p0y - p1y;
    int64_t r = rtx*rtx + rty*rty;

    // calculate 's' values
    int64_t sx = -p0x + -p1x;
    int64_t sy = -p0y + -p1y;

    int32_t count = 0;

    for (Edge e : edges)
    {
        TVert v = verts[e.i0];
        int64_t tx = (2 * static_cast<int64_t>(v.x) + sx);
        int64_t ty = (2 * static_cast<int64_t>(v.y) + sy);
        if (4 * (tx*tx + ty*ty) < r) ++count;
    }

    return count;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::TriangulateEdges(bool use_cdt)
{
    using namespace std;
    using std::begin;
    using std::end;

    // add edges to bounds/tri_edges
    for (Edge e : edges)
    {
        bounds.emplace_back(e.i0, e.i1);
        tri_edges.emplace_back(e.i0, e.i1);
    }

    // create inner segments
    for (Edge e : edges)
    {

        // convenience variables
        size_t i0 = e.i0;
        size_t i1 = e.i1;
        size_t i2 = edges[e.pe].i0;
        TVert p0 = verts[i0];
        TVert p1 = verts[i1];
        TVert p2 = verts[i2];

        for (Edge f : edges)
        {

            size_t i3 = f.i0;
            if (i3 == i0 || i3 == i1 || i3 == i2) continue;
            TVert p3 = verts[i3];

            // test for valid segment
            if (IsContained(p0, p1, p2, p3) && !IdenticalSegment(i0, i3) && !BoundIntersection(p0, p3))
            {
                int32_t len = SqrdLen(p0, p3);													// not the true length, but we don't need the square root
                int32_t inscribe = (use_cdt) ? GetInscribeCount(p0, p3) : 0;		// only calculate cdt factor if necessary
                segs.emplace_back(i0, i3, len, inscribe);
            }
        }
    }

    // sort
    sort(begin(segs), end(segs),
         [](const LineSegment& l0, const LineSegment& l1) -> bool
    {
        return l0.inscribe < l1.inscribe || l0.inscribe == l1.inscribe && l0.length < l1.length;
    }
        );

    // create inner tri_edges
    for (LineSegment s : segs)
    {
        size_t i0 = s.i0;
        size_t i1 = s.i1;
        if (!BoundIntersection(verts[i0], verts[i1]))
        {
            bounds.emplace_back(i0, i1);
            tri_edges.emplace_back(i0, i1);
            tri_edges.emplace_back(i1, i0);
        }
    }
}


// ----- CreateTris -----
template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::ConstructTri(std::vector<TriEdge>::iterator i)
{
    using namespace std;

    auto edge_begin = tri_edges.begin();
    auto edge_end = tri_edges.end();

    TVert p0 = verts[i->i0];

    auto range1 = equal_range(edge_begin, edge_end, TriEdge(i->i1, 0),
                              [](TriEdge e0, TriEdge e1)
    {
        return e0.i0 < e1.i0;
    }
                             );

    for (auto j = range1.first; j != range1.second; ++j)
    {
        if (j->in_use == false) continue;
        TVert p1 = verts[j->i0];

        auto range2 = equal_range(edge_begin, edge_end, TriEdge(j->i1, i->i0),
                                  [](TriEdge e0, TriEdge e1)
        {
            return e0.i0 < e1.i0 || e0.i0 == e1.i0 && e0.i1 < e1.i1;
        }
                                 );

        for (auto k = range2.first; k != range2.second; ++k)
        {
            if (k->in_use == false) continue;
            TVert p2 = verts[k->i0];
            if (GetTriSign(p0, p1, p2) > 0)
            {
                tris.emplace_back(i->i0, j->i0, k->i0, 0);
                i->in_use = false;
                j->in_use = false;
                k->in_use = false;
                return;
            }
        }
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTris()
{
    using namespace std;
    using std::begin;
    using std::end;

    // sanity check
    if (tri_edges.size() < 3) return;

    // sort for fast searching
    sort(begin(tri_edges), end(tri_edges),
         [](TriEdge e0, TriEdge e1)
    {
        return e0.i0 < e1.i0 || e0.i0 == e1.i0 && e0.i1 < e1.i1;
    }
        );

    // construct tris
    for (auto i = begin(tri_edges); i != end(tri_edges); ++i)
    {
        if (i->in_use) ConstructTri(i);
    }
}


// ----- RemoveUnusedVerts -----
template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::SwapTriVertex(size_t i0, size_t i1)
{
    for (TTri& t : tris)
    {
        if (static_cast<size_t>(t.i0) == i0)
        {
            t = TTri(i1, t.i1, t.i2, t.coef);
        }
        else if (static_cast<size_t>(t.i0) == i1)
        {
            t = TTri(i0, t.i1, t.i2, t.coef);
        }
        if (static_cast<size_t>(t.i1) == i0)
        {
            t = TTri(t.i0, i1, t.i2, t.coef);
        }
        else if (static_cast<size_t>(t.i1) == i1)
        {
            t = TTri(t.i0, i0, t.i2, t.coef);
        }
        if (static_cast<size_t>(t.i2) == i0)
        {
            t = TTri(t.i0, t.i1, i1, t.coef);
        }
        else if (static_cast<size_t>(t.i2) == i1)
        {
            t = TTri(t.i0, t.i1, i0, t.coef);
        }
    }
}

template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::VertexUseCount(size_t i) const
{
    size_t c = 0;
    for (TTri t : tris)
    {
        if (t.i0 == i) ++c;
        if (t.i1 == i) ++c;
        if (t.i2 == i) ++c;
    }
    return c;
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::VertexInUse(size_t i) const
{
    for (TTri t : tris)
    {
        if (t.i0 == i || t.i1 == i || t.i2 == i) return true;
    }
    return false;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::RemoveUnusedVerts()
{
    // removes any unused vertices
    // only applies to tris, does not change the indices of edges, segments, ect...
    // should only be used at the end
    for (size_t i = 0; i < verts.size();)
    {
        if (!VertexInUse(i))
        {
            SwapTriVertex(i, verts.size() - 1);
            std::swap(verts[i], verts.back());
            verts.pop_back();
        }
        else ++i;
    }
}


// ----- modifier functions -----
template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::ApplyBold()
{
    // must be called after tri's are constructed

    // copy verts to temporary
    bverts = verts;

    // move vertices
    for (Edge e0 : edges)
    {
        Edge e1 = edges[e0.ne];
        verts[e0.i1] = bverts[e0.i1] + ScaledPerpCCW(bverts[e0.i0], bverts[e0.i1], bverts[e1.i1], bold_offset);
    }

    for (TTri t : tris)
    {
        // inner curves and non-curve edges are already handled in the edge loop above
        // just need to handle outer curve control points
        if (t.coef == +1)
        {
            verts[t.i1] = bverts[t.i1] + ScaledPerpCCW(bverts[t.i0], bverts[t.i1], bverts[t.i2], bold_offset);
        }
    }

    // handle self intersections??
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::ApplyItalic()
{
    for (TVert& v : verts) v.x += (v.y * italic_offset_x) / italic_offset_y;
}


// ----- triangulation functions -----



// ----- debug functions -----
template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::VerifyEdgeList()
{
    for (size_t i = 0; i < edges.size(); ++i)
    {
        Edge e = edges[i];
        Edge pe = edges[e.pe];
        Edge ne = edges[e.ne];
        if (e.i0 != pe.i1 || e.i1 != ne.i0)
        {
            return i;
        }
        if (pe.ne != i || ne.pe != i)
        {
            return i;
        }
    }
    return index_error;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug()
{

    for (TriEdge te : tri_edges)
    {

        // create offsets
        TVert p0 = verts[te.i0];
        TVert p1 = verts[te.i1];
        TVert d = p1 - p0;
        TVert off0 = ScaledNormal(vec2t(-d.y, d.x), 6);
        TVert off1 = ScaledNormal(vec2t(d.y, -d.x), 6);

        // create new vertices
        verts.push_back(p0 + off0);
        verts.push_back(p0 + off1);
        verts.push_back(p1 + off0);
        verts.push_back(p1 + off1);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 1, sz - 3, sz - 4, 0));
        tris.push_back(TTri(sz - 1, sz - 4, sz - 2, 0));
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug2(int16_t line_width)
{

    for (Edge e : edges)
    {

        // create offsets
        TVert p0 = verts[e.i0];
        TVert p1 = verts[e.i1];
        if (p0 == p1) continue;
        TVert d = p1 - p0;
        TVert off0 = ScaledNormal(vec2t(-d.y, d.x), line_width);
        TVert off1 = ScaledNormal(vec2t(d.y, -d.x), line_width);

        // create new vertices
        verts.push_back(p0 + off0);
        verts.push_back(p0 + off1);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 1, sz - 2, e.i1, 0));
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug3(int16_t line_width)
{

    for (LineSegment s : segs)
    {

        // create offsets
        TVert p0 = verts[s.i0];
        TVert p1 = verts[s.i1];
        if (p0 == p1) continue;
        TVert d = p1 - p0;
        TVert off0 = ScaledNormal(vec2t(-d.y, d.x), line_width);
        TVert off1 = ScaledNormal(vec2t(d.y, -d.x), line_width);

        // create new vertices
        verts.push_back(p0 + off0);
        verts.push_back(p0 + off1);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 1, sz - 2, s.i1, 0));
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug4(int16_t width)
{

    // create offset vectors
    TVert off0 = ScaledNormal(vec2t(-1, -1), width);
    TVert off1 = ScaledNormal(vec2t(0, +1), width);
    TVert off2 = ScaledNormal(vec2t(+1, -1), width);

    // create edge tris
    size_t count = verts.size();
    for (size_t i = 0; i < count; ++i)
    {

        TVert v = verts[i];

        // create new vertices
        verts.push_back(v + off0);
        verts.push_back(v + off1);
        verts.push_back(v + off2);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 3, sz - 2, sz - 1, 0));
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug5(int16_t line_width)
{

    // create edge tris
    for (Bound b : bounds)
    {

        // create offsets
        TVert p0 = verts[b.i0];
        TVert p1 = verts[b.i1];
        TVert d = p1 - p0;
        if (p0 == p1) continue;
        TVert off0 = ScaledNormal(vec2t(-d.y, d.x), line_width);
        TVert off1 = ScaledNormal(vec2t(d.y, -d.x), line_width);

        // create new vertices
        verts.push_back(p0 + off0);
        verts.push_back(p0 + off1);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 1, sz - 2, b.i1, 0));
    }
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::CreateTrisDebug6(int16_t line_width, bool in_use)
{

    for (TriEdge te : tri_edges)
    {

        if (te.in_use != in_use) continue;

        // create offsets
        TVert p0 = verts[te.i0];
        TVert p1 = verts[te.i1];
        TVert d = p1 - p0;
        //if (p0 == p1) continue;
        TVert off0 = ScaledNormal(vec2t(-d.y, d.x), line_width);
        TVert off1 = ScaledNormal(vec2t(d.y, -d.x), line_width);

        // create new vertices
        verts.push_back(p0 + off0);
        verts.push_back(p0 + off1);

        // add tris
        size_t sz = verts.size();
        tris.push_back(TTri(sz - 1, sz - 2, te.i1, 0));
    }
}


// ----- Triangulator private interface -----
template <typename TVert, typename TTri> std::vector<ContourPoint>& Triangulator2D<TVert, TTri>::GetContours()
{
    return contours;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::AppendTriangulation()
{
    // Clear() has been called,  GetContours() called and the contour points filled

    TraceContour();
    SubdivideCurves();
    TriangulateEdges((flags & TriangulatorFlags::use_cdt) == TriangulatorFlags::use_cdt);
    CreateTris();

    if (bold_offset != 0) ApplyBold();
    if (italic_offset_x != 0 && italic_offset_y != 0) ApplyItalic();

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


// ----- Triangulator interface -----
template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::Clear()
{
    contours.clear();
    verts.clear();
    bverts.clear();
    edges.clear();
    segs.clear();
    bounds.clear();
    tri_edges.clear();
    tris.clear();
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::Trim()
{
    contours.clear();
    contours.shrink_to_fit();
    verts.clear();
    verts.shrink_to_fit();
    bverts.clear();
    bverts.shrink_to_fit();
    edges.clear();
    edges.shrink_to_fit();
    segs.clear();
    segs.shrink_to_fit();
    bounds.clear();
    bounds.shrink_to_fit();
    tri_edges.clear();
    tri_edges.shrink_to_fit();
    tris.clear();
    tris.shrink_to_fit();
}


// ----- mesh access -----
template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::GetVertexCount() const
{
    return verts.size();
}

template <typename TVert, typename TTri> size_t Triangulator2D<TVert, TTri>::GetTriangleCount() const
{
    return tris.size();
}

template <typename TVert, typename TTri> TVert Triangulator2D<TVert, TTri>::GetVertex(size_t i) const
{
    return verts[i];
}

template <typename TVert, typename TTri> TTri Triangulator2D<TVert, TTri>::GetTriangle(size_t i) const
{
    return tris[i];
}

template <typename TVert, typename TTri> const TVert* Triangulator2D<TVert, TTri>::GetVertexRaw() const
{
    return verts.data();
}

template <typename TVert, typename TTri> const TTri* Triangulator2D<TVert, TTri>::GetTriangleRaw() const
{
    return tris.data();
}

template <typename TVert, typename TTri> TVert Triangulator2D<TVert, TTri>::operator[](size_t i) const
{
    return verts[i];
}

template <typename TVert, typename TTri> const TTri* Triangulator2D<TVert, TTri>::begin() const
{
    return tris.data();
}

template <typename TVert, typename TTri> const TTri* Triangulator2D<TVert, TTri>::end() const
{
    return tris.data() + tris.size();
}

template <typename TVert, typename TTri> bool Triangulator2D<TVert, TTri>::Isvalid() const
{
    // this'll work for now, I'll do something more complex if I ever decide to add more triangle types

    if (std::is_same<TTri,TriSmall>::value)
    {
        return verts.size() <= 0x3ff;
    }

    if (std::is_same<TTri,TriLarge>::value)
    {
        return verts.size() <= 0xffff;
    }

    return true;
}


// ----- flags access -----
template <typename TVert, typename TTri> TriangulatorFlags Triangulator2D<TVert, TTri>::GetFlags() const
{
    return flags;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::SetFlags(TriangulatorFlags flags_)
{
    flags = flags_;
}

template <typename TVert, typename TTri> typename Triangulator2D<TVert, TTri>::TCoord Triangulator2D<TVert, TTri>::GetBold() const
{
    return bold_offset;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::SetBold(TCoord bold_)
{
    bold_offset = bold_;
}

template <typename TVert, typename TTri> typename Triangulator2D<TVert, TTri>::TCoord Triangulator2D<TVert, TTri>::GetItalicX() const
{
    return italic_offset_x;
}

template <typename TVert, typename TTri> typename Triangulator2D<TVert, TTri>::TCoord Triangulator2D<TVert, TTri>::GetItalicY() const
{
    return italic_offset_y;
}

template <typename TVert, typename TTri> void Triangulator2D<TVert, TTri>::SetItalic(TCoord ix, TCoord iy)
{
    italic_offset_x = ix;
    italic_offset_y = iy;
}


// ---------------------------------------------------------------------------------------------------------------------------
//	End
// ---------------------------------------------------------------------------------------------------------------------------

}			// end of TTFCore namespace

