#pragma once

#include "app.h"

struct evkeyvalq;
struct evhttp_request;
typedef struct json_t json_t;

/**
 * Wrapper of path, args and form.
 */
struct cqhttp_request {
    const char* path;
    const struct evkeyvalq* args;
    const struct evkeyvalq* form;
    json_t* json;
};

/**
 * Generic handler for evhttp.
 */
void cqhttp_main_handler(struct evhttp_request* req, void* _);

enum cqhttp_retcode {
    CQHTTP_RETCODE_OK = 0,
    CQHTTP_RETCODE_ERROR_DEFAULT = 100,
    CQHTTP_RETCODE_NO_SUCH_API = 101, // will be converted to 404 Not Found http error
    CQHTTP_RETCODE_INVALID_DATA = 102 // the data that CoolQ returns is invalid
};

struct cqhttp_result {
    cqhttp_result() : retcode(CQHTTP_RETCODE_ERROR_DEFAULT), data(NULL) {};
    int retcode; // succeeded: 0, lack of parameters or invalid ones: 1, CQ error code: -11, -23, etc... (< 0)
    json_t* data;
};

/**
 * Get char string parameter from url arguments, or form, or json.
 * The return value should be freed by caller.
 */
char *cqhttp_get_param(const struct cqhttp_request &request, const char *key);

/**
 * Get string parameter from url arguments, or form, or json. Return default_val if no such key.
 */
str cqhttp_get_str_param(const struct cqhttp_request &request, const char *key, const str &default_val = "");

/**
 * Like cqhttp_get_param, but parse integer if possible. Return default_val if it's not a number.
 */
int64_t cqhttp_get_integer_param(const struct cqhttp_request &request, const char *key, int64_t default_val = 0);

/**
 * Like cqhttp_get_param, but parse bool if possible. Return default_val if it's not a boolean.
 */
bool cqhttp_get_bool_param(const struct cqhttp_request &request, const char *key, bool default_val = false);

typedef struct cqhttp_result(*cqhttp_request_handler)(const struct cqhttp_request& request);

#include <cstring>

struct c_str_comparator {
    bool operator()(char const* a, char const* b) const {
        return strcmp(a, b) < 0;
    }
};

#include <map>

typedef std::map<const char *, cqhttp_request_handler, c_str_comparator> cqhttp_request_handler_map;
