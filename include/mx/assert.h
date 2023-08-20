#ifndef _MX_ASSERT_H_
#define _MX_ASSERT_H_

#include <assert.h>
#include <mx/base.h>

#define MX_ASSERT(cond, msg)        assert((cond) && ""msg)
#define MX_ASSERT_PTR(ptr, message) assert((ptr) && ""message)
#define MX_ASSERT_SELFPTR(ptr)      MX_ASSERT_PTR(ptr, "Instance pointer "#ptr" must be valid.")
#define MX_ASSERT_OOM(ptr)          MX_ASSERT_PTR(ptr, "Out of memory.")

#endif
