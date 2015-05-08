/* MSVC does not know roundf() */
#include <math.h>
static inline float roundf(float x)
{
    return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
