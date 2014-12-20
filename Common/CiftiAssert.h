#ifndef __CARET_ASSERT_H__
#define __CARET_ASSERT_H__

//some minimal adaptation to cassert
#include <cassert>

#define CiftiAssert(i) assert(i)

#define CiftiAssertVectorIndex(v, i) assert((i) >= 0 && (i) < static_cast<int64_t>(v.size()))

#endif //__CARET_ASSERT_H__
