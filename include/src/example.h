#ifndef REI_WOLF_EXAMPLE_H
#define REI_WOLF_EXAMPLE_H

#include "system.h"

typedef struct EXAMPLE_Data {
    Vec2 example_xy;
    Vec3 example_xyz;
    char const* example_string;
} EXAMPLE_Data;

EXAMPLE_Data* EXAMPLE_my_example_create(f32 x1, f32 y1, f32 x2, f32 y2, f32 z2, const char* string);
void EXAMPLE_my_example_init(EXAMPLE_Data* data);
bool EXAMPLE_my_example_increment_Vec2(EXAMPLE_Data* data, size_t increment);
bool EXAMPLE_my_example_increment_Vec3(EXAMPLE_Data* data, size_t increment);
void EXAMPLE_my_example_destroy(EXAMPLE_Data** data);

#endif