#ifndef SPP_UTILS_H
#define SPP_UTILS_H

#include <errno.h>

#define errno_push()  do { \
	                      int spp_internal_errno_push_tmp; \
	                      ((void)(spp_internal_errno_push_tmp = errno))
#define errno_pop()   \
	                      (void)(errno = spp_internal_errno_push_tmp); \
                      } while(0)

#ifdef NDEBUG
	#define debug_errno_push() do {
	#define debug_errno_pop()  } while(0)
#else
	#define debug_errno_push() errno_push()
	#define debug_errno_pop()  errno_pop()
#endif

#endif /* SPP_UTILS_H */
