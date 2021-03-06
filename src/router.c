#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "router.h"


#define MAX_ROUTES (128)
#define MAX_HANDLERS (8)

struct route {
	method_t method;
	const char* path;
	handle_t* handlers[MAX_HANDLERS + 1];
} routes[MAX_ROUTES];
int n = 0;

bool pathMatch(const char* routePath, const char* path) {
	size_t routePathLength = strlen(routePath);
	size_t pathLength = strlen(path);

	bool matches = true;
	// need to go 1 char further for the path length, because of wildcards
	// shouldn't break anything as there has to be a 0-byte anyway
	for(size_t i = 0, j = 0; i < routePathLength && j < pathLength + 1; i++, j++) {
		if (routePath[i] == '*') {
			if (routePath[i + 1] == '*') {
				break;
			}
			if (path[j] == '/') {
				if (routePath[i + 1] == '/') {
					// continue without i--; match next path char against *
				} else {
					// no match
					matches = false;
					break;				
				}
			} else {
				// matches this char in path, try next
				i--;
			}
			continue;
		}
		if (routePath[i] == path[j]) {
			// matches; go to next
		} else {
			// no match
			matches = false;
			break;
		}
		
		if (j == pathLength - 1 && i == routePathLength - 1) {
			// both path are finished with no conflict
			break;
		}
		if ((j == pathLength) != (i == routePathLength - 1)) {
			// path length is exhausted but not the route path
			// or route path is exhausted but not the path
			// we are not in a wildcard
			// => no match
			matches = false;
			break;
		}
	}
	
	return matches;
}

struct route* findRoute(method_t method, const char* path) {
	for (int i = 0; i < n; i++) {
		if (routes[i].method == method && pathMatch(routes[i].path, path)) {
			return &routes[i];
		}
	}
	
	return NULL;
}

struct route* reverseFindRoute(method_t method, const char* path) {
	for (int i = 0; i < n; i++) {
		if (routes[i].method == method && pathMatch(path, routes[i].path)) {
			return &routes[i];
		}
	}
	
	return NULL;
}

int registerRoute(method_t method, const char* path, ...) {
	if (n >= MAX_ROUTES) {
		return -1;
	}
	
	if (findRoute(method, path) != NULL || reverseFindRoute(method, path) != NULL) {
		return -2;
	}
		
	struct route route = {
		method: method,
		path: path
	};
	
	va_list argptr;
	va_start(argptr, path);
	
	int i;
	for(i = 0; i < MAX_HANDLERS; i++) {
		route.handlers[i] = va_arg(argptr, handle_t*);
		if (route.handlers[i] == NULL) {
			break;
		}
	}
	route.handlers[i] = NULL;
	
	va_end(argptr);
	
	routes[n++] = route;
	
	return 0;
}

response_t routerHandler(ctx_t* ctx) {
	struct route* route = findRoute(ctx->method, ctx->path);
	if (route == NULL) {
		return errorResponse(404, "no route found");
	}
	
	for(int i = 0; route->handlers[i] != NULL; i++) {
		response_t response = route->handlers[i](ctx);
		if (response.status != NEXT_RESPONSE_STATUS) {
			return response;
		}
	}
	
	return errorResponse(500, "no handler produced valid response");
}
