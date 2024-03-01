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

BRICKS_API int
bricks_get_node_children_count(bricks_handle_t h, const char* p);

/*
BRICKS_API const char*
bricks_get_node_property(bricks_handle_t h, const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_value(bricks_handle_t h, const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_value_as_int(bricks_handle_t h, const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_value_as_bool(bricks_handle_t h, const char* c, const char* p);

BRICKS_API const char*
bricks_get_node_property_by_index(bricks_handle_t h, const char* c, int index, const char* p);
*/

#ifdef __cplusplus
}
#endif
