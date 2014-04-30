#ifndef __CARET_ASSERT_H__
#define __CARET_ASSERT_H__

//some minimal adaptation to cassert
#include <cassert>

#define CaretAssert(i) assert(i)

#define CaretAssertVectorIndex(v, i) assert((i) >= 0 && (i) < static_cast<int64_t>(v.size()))

#endif //__CARET_ASSERT_H__
