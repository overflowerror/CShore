#include <stdlib.h>
#include <string.h>

#include <controller.h>
#include <cookies.h>

#include "entities.h"


GET("/", hello);
GET("/index.*", hello);
response_t hello(ctx_t* ctx) {
	return rawResponse(200, "Hello World\n");
}

GET("/foobar", foobar);
response_t foobar(ctx_t* ctx) {
	return fileResponse("demo/foobar.txt");
}

response_t authenticate(ctx_t* ctx) {
	if (ctx->auth.type != BASIC) {
		return basicAuthResponse(401, "Protected Area");
	}
	
	if (strcmp(ctx->auth.basic.user, "admin") != 0 ||
	    strcmp(ctx->auth.basic.password, "password") != 0
	) {
		// username or password wrong
		return basicAuthResponse(401, "Protected Area");
	}

	return next();
}

GET("/user", authenticate, user);
POST("/user", authenticate, user);
response_t user(ctx_t* ctx) {
	user_t user = {
		.username = "overflowerror",
		.github = "https://github.com/overflowerror"
	};

	return jsonResponse(200, user_t, &user);
}

GET("/template", template);
response_t template(ctx_t* ctx) {
	return templateResponse(200, "demo.templ", "Page Title", "Overflow");
}

GET("/cookies", cookies);
response_t cookies(ctx_t* ctx) {
	char* test = getCookie(ctx, "test");
	
	setCookie(ctx, "test", "foobar", cookieSettingsNull());
	
	if (test == NULL) {
		return rawResponse(200, "cookie not set");
	} else if (strcmp(test, "foobar") == 0) {
		free(test);
		return rawResponse(200, "cookie value correct");
	} else {
		free(test);
		return rawResponse(200, "cookie value incorrect");
	}
}
