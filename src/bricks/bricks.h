#pragma once
#ifndef SWIG                            
	#ifdef BRICKS_EXPORTS
	#define BRICKS_API __declspec(dllexport)
	#else
	#define BRICKS_API __declspec(dllimport)
	#endif
#else
	#define BRICKS_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BRICKS_INTERFACE_VERSION "0.1.0"

typedef uintptr_t bricks_handle_t;
enum bricks_error_code_e
{
	BRICKS_SUCCESS = 0,
	BRICKS_FAILURE_GENERIC = 1,
	BRICKS_INVALID_PARAM = 2
};

/* logs */
typedef BRICKS_API void (*logger_t) 
(const char*);

void BRICKS_API 
brick_set_logger(logger_t);

/* structured data interface */
BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_file(const char* f);

BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_string(const char* s);

BRICKS_API int
bricks_get_node_children_count(bricks_handle_t h, const char* p);


BRICKS_API const char* 
bricks_get_node_property(bricks_handle_t h , const char* c, const char* p);

BRICKS_API const char* 
bricks_get_node_value(bricks_handle_t h , const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_value_as_int(bricks_handle_t h, const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_value_as_bool(bricks_handle_t h , const char* c, const char* p);

BRICKS_API const char* 
bricks_get_node_property_by_index(bricks_handle_t h , const char* c, int index, const char* p);



/* discoverer interface */

/* service interface */


BRICKS_API enum bricks_error_code_e bricks_open(const char* xml, bricks_handle_t **h);

BRICKS_API enum bricks_error_code_e bricks_poll(bricks_handle_t* h);





#ifdef __cplusplus
}
#endif
