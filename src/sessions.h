#ifndef SESSIONS_H_
#define SESSIONS_H_

#ifndef SESSION_PTR_TYPE
	#warning "session ptr type not defined"
	#define SESSION_PTR_TYPE int
#endif

#ifndef SESSION_LENGTH
	#define SESSION_LENGTH (24*60*60*1000)
#endif

#ifndef SESSION_COOKIE_NAME
	#define SESSION_COOKIE_NAME "cshore_session"
#endif

#include "request.h"

void* _session_start(ctx_t*, const char*, size_t);

#define session_start(c) (SESSION_PTR_TYPE*) _session_start(c, SESSION_COOKIE_NAME, sizeof(SESSION_PTR_TYPE))

#endif
