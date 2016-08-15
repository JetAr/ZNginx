#include "ElMaxPlugin.h"

void ElMaxPlugin::DumpMatrix3(Matrix3* m)
{
	Point3 row;

	// Dump the whole Matrix
	row = m->GetRow(0);
	outfile << indent.str() << row.x << ' ' << row.y << ' ' << row.z << endl;
	row = m->GetRow(1);
	outfile << indent.str() << row.x << ' ' << row.y << ' ' << row.z << endl;
	row = m->GetRow(2);
	outfile << indent.str() << row.x << ' ' << row.y << ' ' << row.z << endl;
	row = m->GetRow(3);
	outfile << indent.str() << row.x << ' ' << row.y << ' ' << row.z << endl;
}

// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
BOOL ElMaxPlugin::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0), m.GetRow(1)), m.GetRow(2)) < 0.0f) ? 1 : 0;
}