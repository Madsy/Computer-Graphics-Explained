#ifndef MYASSERT_H_GUARD
#define MYASSERT_H_GUARD

#ifdef DEBUG
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif
#endif

