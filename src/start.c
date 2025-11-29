#define MEX_SYSTEM_INCLUDE
#include "system.h"
#undef MEX_SYSTEM_INCLUDE

#include "example.h"
#include "modify_static_variable.h"

static unsigned int my_static_var = 0;
static unsigned int const my_static_const_var = 0;
static unsigned int my_static_var2 = 9;
static unsigned int const my_static_const_var2 = 9;

static bool first_run = false;

void other() {
    my_static_var++;
    MODIFY_up_the_volume();
}

void start() {

    if (!first_run) return;

    static signed char my_static_var3 = 0;
    static signed char const my_static_const_var3 = 0;
    static signed char my_static_var4 = 9;
    static signed char const my_static_const_var4 = 9;

    my_static_var3++;

    EXAMPLE_Data* data_ptr = EXAMPLE_my_example_create(1, 2, 3, 4, 5, "yippee");
    EXAMPLE_Data data_instance;

    if (EXAMPLE_my_example_increment_Vec2(data_ptr, 4)) {
        my_static_var3 = my_static_const_var;
    }

    other();

    my_static_var4++;

    if (EXAMPLE_my_example_increment_Vec3(data_ptr, -4)) {
        my_static_var4 = my_static_const_var2;
    }

    EXAMPLE_my_example_init(&data_instance);
    EXAMPLE_my_example_destroy(&data_ptr);
    first_run = true;
}
