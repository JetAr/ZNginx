// *************************************************************************************
//	TTFTriangulator2D.cpp
//
//		Implementation of the TTF Triangulator2D.
//
//																																Ryan Bogean
//																																March 2015
//
// *************************************************************************************

# include <exception>
# include <vector>
# include <cstdint>

# include "TTFExceptions.h"
# include "TTFMath.h"
# include "TTFTypes.h"
# include "TTFTriangulator2D.h"

using namespace TTFCore;


// ---------------------------------------------------------------------------------------------------------------------------
//	TriSmall
// ---------------------------------------------------------------------------------------------------------------------------

TriSmall::TriSmall(size_t i0_, size_t i1_, size_t i2_, int32_t coef_)
{
    i0 = static_cast<uint32_t>(i0_);
    i1 = static_cast<uint32_t>(i1_);
    i2 = static_cast<uint32_t>(i2_);
    coef = coef_;
}


TriLarge::TriLarge(size_t i0_, size_t i1_, size_t i2_, int16_t coef_)
{
    i0 = static_cast<uint16_t>(i0_);
    i1 = static_cast<uint16_t>(i1_);
    i2 = static_cast<uint16_t>(i2_);
    coef = coef_;
}
