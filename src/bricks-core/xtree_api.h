#pragma once
#include "bricks_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* structured data interface */
BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_file(const char* f);

BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_string(const char* s);

BRICKS_API void
bricks_destroy_xtree(bricks_handle_t);

BRICKS_API int
bricks_get_node_children_count(bricks_handle_t h, const char* p);

BRICKS_API const char*
bricks_get_node_value(bricks_handle_t h, const char* np);

BRICKS_API const char*
bricks_get_node_property(bricks_handle_t h, const char* np, const char* pp);

BRICKS_API const char*
bricks_get_child_value_by_index(bricks_handle_t h, const char* np, int i);

BRICKS_API const char*
bricks_get_child_property_by_index(bricks_handle_t h, const char* np, int i, const char* pp);

BRICKS_API int
bricks_get_node_property_as_int(bricks_handle_t h, const char* np, const char* pp);

BRICKS_API int
bricks_get_child_property_as_int_by_index(bricks_handle_t h, const char* np, int i, const char* pp);

BRICKS_API bool
bricks_get_node_property_as_bool(bricks_handle_t h, const char* np, const char* pp);

BRICKS_API bool
bricks_get_child_property_as_bool_by_index(bricks_handle_t h, const char* np, int i, const char* pp);

BRICKS_API double
bricks_get_node_property_as_double(bricks_handle_t h, const char* np, const char* pp);

BRICKS_API double
bricks_get_child_property_as_double_by_index(bricks_handle_t h, const char* np, int i, const char* pp);


#ifdef __cplusplus
}
#endif
