%module bricks
%{
#include "bricks.h"
%}

#define BRICKS_INTERFACE_VERSION "0.1.0"

extern void bricks_register_service(void);

