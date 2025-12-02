#include "system.h"

#include "minor_scene_bootstrapper.h"
#include "demo_css.h"
#include "alloc.h"

static MSB_CallbackExisting* callbacks = NULL;
static u16* think_count = NULL;

static bool has_displayed_css_think_message_yet = false;

void CSS_Think(void) {
    if (!think_count || !callbacks) {
        OSReport("[Rei Wolf]: think_count (%p) OR callbacks (%p) NOT SET!", think_count, callbacks);
        return;
    }

    if (*think_count < 5) {
        OSReport("[Rei Wolf]: think_count: %hu...", *think_count);
    } else if (*think_count == 5) {
        OSReport("[Rei Wolf]: think_count: %hu! Ok that's enough counting for now.", *think_count);
    }

    if (*think_count < 100) (*think_count)++; // Stop counting after thinking 100 times

    if (callbacks->MSB_ExistingThink) {
        if (!has_displayed_css_think_message_yet) {
            OSReport("[Rei Wolf]: Attempting to call original think function: void %p()", callbacks->MSB_ExistingLoad);
            has_displayed_css_think_message_yet = true;
        }
        callbacks->MSB_ExistingThink();
    }
}

void CSS_Load(void* data) {
    OSReport("[Rei Wolf]: Loading CSS hook!");

    if (!think_count) {
        OSReport("[Rei Wolf]: think_count not set! Setting...");
        think_count = INJECT_Alloc(sizeof(*think_count));
        *think_count = 0;
        OSReport("[Rei Wolf]: think_count set at: %p", think_count);
    } else {
        OSReport("[Rei Wolf]: think_count already set??? What?!");
    }

    if (callbacks->MSB_ExistingLoad) {
        OSReport("[Rei Wolf]: Attempting to call original load function with parameter(s): void %p(void* %p)", callbacks->MSB_ExistingLoad, data);
        callbacks->MSB_ExistingLoad(data);
    }
}

void CSS_Exit(void* data) {
    OSReport("[Rei Wolf]: Exiting CSS hook!");

    INJECT_Free(think_count);
    think_count = NULL;

    if (callbacks->MSB_ExistingExit) {
        OSReport("[Rei Wolf]: Attempting to call original exit function with parameter(s): void %p(void* %p)", callbacks->MSB_ExistingExit, data);
        callbacks->MSB_ExistingExit(data);
    }
}

void CSS_Bootstrap() {
    if (callbacks) {
        OSReport("[Rei Wolf]: CSS Bootstrap was attempted twice??? How is callbacks set...");
        return;
    }

    callbacks = INJECT_Alloc(sizeof(MSB_CallbackExisting));

    MSB_Bootstrap(MINOR_SCENE_CSS, CSS_Think, CSS_Load, CSS_Exit, callbacks);
}
