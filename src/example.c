#include "example.h"

static const char* EXAMPLE_default_string = "default";

EXAMPLE_Data* EXAMPLE_my_example_create(f32 x1, f32 y1, f32 x2, f32 y2, f32 z2, const char* string) {
    EXAMPLE_Data* data = HSD_MemAlloc(sizeof(EXAMPLE_Data));
    char* string_alloc = HSD_MemAlloc(strlen(string));
    strcpy(string_alloc, string);
    data->example_string = string_alloc;
    data->example_xy = (Vec2){ .X = x1, .Y = y1 };
    data->example_xyz = (Vec3){ .X = x2, .Y = y2, .Z = z2 };
    return data;
}

void EXAMPLE_my_example_init(EXAMPLE_Data* data) {
    data->example_xy.X = 0.0f;
    data->example_xy.Y = 0.0f;
    data->example_xyz.X = 0.0f;
    data->example_xyz.Y = 0.0f;
    data->example_xyz.Z = 0.0f;
    data->example_string = EXAMPLE_default_string;
}

bool EXAMPLE_my_example_increment_Vec2(EXAMPLE_Data* data, size_t increment) {
    Vec2 *v = &data->example_xy;
    v->X += increment;
    v->Y += increment;
    return (v->X + v->Y) > 0.0f;
}

bool EXAMPLE_my_example_increment_Vec3(EXAMPLE_Data* data, size_t increment) {
    Vec3 *v = &data->example_xyz;
    v->X += increment;
    v->Y += increment;
    v->Z += increment;
    return (v->X + v->Y + v->Z) > 0.0f;
}

void EXAMPLE_my_example_destroy(EXAMPLE_Data** data) {
    HSD_Free((void*)(*data)->example_string);
    (*data)->example_string = NULL;
    HSD_Free(*data);
    *data = NULL;
}