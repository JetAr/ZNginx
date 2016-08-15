#ifndef __ElDefines_H__
#define __ElDefines_H__

#define ElNew new
#define ElDelete delete
#define ElSafeRelease(x) if ((x)) { (x)->Release(); (x) = NULL; }
#define ElSafeDelete(x) if ((x)) { delete (x); (x) = NULL; }
#define ElSafeDeleteArray(x) if ((x)) { delete[] (x); (x) = NULL; }

#define EL_MAX_PATH	MAX_PATH

// defines for small numbers
#define EPSILON_E3 (float)(1E-3)
#define EPSILON_E4 (float)(1E-4)
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)
#define float_equal(a, b) ((fabs((a)-(b)) < EPSILON_E6) ? true : false)
#define lerp(a, b, t) ((a) * (1.0f - (t)) + ((b) * (t)))
#define clamp(a, b, c) (min(max((a), (b)), (c)))

typedef int TimeValue;
typedef DWORD ColorValue;

#endif //__ElDefines_H__
