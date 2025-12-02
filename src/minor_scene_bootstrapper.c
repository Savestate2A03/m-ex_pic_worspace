#include "system.h"

#include "alloc.h"
#include "minor_scene_bootstrapper.h"

#define MSB_MAX_SIZE 32

// Slightly modified version of MinorSceneDesc from m-ex/structs.h
typedef struct MinorSceneDesc_Modified {
    u32 id: 8;
    u32 unk_x1: 24;
    void (*Think)();
    void (*Load)(void *data);
    void (*Exit)(void *data);
    u32 unk_x14;
} MinorSceneDesc_Modified;

typedef struct MSB_Data {
    MSB_Think think;
    MSB_Load load;
    MSB_Exit exit;
    u8 id: 8;
    bool taken;
} MSB_Data;

typedef struct MinorSceneBootstrapper {
    MSB_Data data[MSB_MAX_SIZE];
} MinorSceneBootstrapper;

// Add 803DA91C:stc_Scene_MinorDescTable to melee.link
extern MinorSceneDesc_Modified stc_Scene_MinorDescTable[0x2D];
static MinorSceneBootstrapper* MSB = NULL;

/** ---------- Private ---------- */

void MSB_Init() {
    OSReport("[Rei Wolf]: Welcome to MSB_Init!");
    if (!MSB) {
        MSB = INJECT_Alloc(sizeof(MinorSceneBootstrapper));
        OSReport("[Rei Wolf]: MSB Allocated: %p", MSB);
    } else {
        OSReport("[Rei Wolf]: MSB_Init called twice???");
        return;
    }

    for (size_t i = 0; i < MSB_MAX_SIZE; i++) {
        MSB->data[i] = (MSB_Data){
            .think = NULL,
            .load = NULL,
            .exit = NULL,
            .id = 0,
            .taken = false
        };
    }
}

size_t MSB_SlotsTaken() {
    size_t slots = 0;
    for (size_t i = 0; i < MSB_MAX_SIZE; i++) {
        if (MSB->data[i].taken) slots++;
    }

    OSReport("[Rei Wolf]: MSB_SlotsTaken: %d", slots);
    return slots;
}

MSB_Data* MSB_GetFreeSlot() {
    for (size_t i = 0; i < MSB_MAX_SIZE; i++) {
        if (!MSB->data[i].taken) {
            OSReport("[Rei Wolf]: MSB_GetFreeSlot: %p", &MSB->data[i]);
            return &MSB->data[i];
        }
    }

    OSReport("[Rei Wolf]: MSB_GetFreeSlot: NULL");
    return NULL;
}

MSB_Data* MSB_GetSavedByID(u8 id) {
    for (size_t i = 0; i < MSB_MAX_SIZE; i++) {
        if (MSB->data[i].id == id) {
            OSReport("[Rei Wolf]: MSB_GetSavedByID: %p", &MSB->data[i]);
            return &MSB->data[i];
        }
    }
    OSReport("[Rei Wolf]: MSB_GetSavedByID: NULL");
    return NULL;
}

MinorSceneDesc_Modified* MSB_GetMinorSceneDescByID(u8 id) {
    size_t elements = sizeof(stc_Scene_MinorDescTable)/sizeof(MinorSceneDesc_Modified);
    for (size_t i = 0; i < elements; i++) {
        if (stc_Scene_MinorDescTable[i].id == id) {
            OSReport("[Rei Wolf]: MSB_GetMinorSceneDescByID: %p", &stc_Scene_MinorDescTable[i]);
            return &stc_Scene_MinorDescTable[i];
        }
    }
    OSReport("[Rei Wolf]: MSB_GetMinorSceneDescByID: NULL");
    return NULL;
}

void MSB_Destroy() {
    OSReport("[Rei Wolf]: Freeing MSB at MSB_Destroy!");
    INJECT_Free(MSB);
    MSB = NULL;
}

/** ---------- Public ---------- */

void MSB_Bootstrap(u8 id, MSB_Think new_think, MSB_Load new_load, MSB_Exit new_exit, MSB_CallbackExisting* callbacks) {
    OSReport("[Rei Wolf]: HELLO DO YOU WANT TO BOOTSTRAP AN INJECT INTO A MINOR SCENE?!");
    OSReport("[Rei Wolf]: id: %hhu, new_think: %p, new_load: %p, new_exit: %p, callbacks: %p (to fill in)", id, new_think, new_load, new_exit, callbacks);

    *callbacks = (MSB_CallbackExisting){
        .MSB_ExistingThink = NULL,
        .MSB_ExistingLoad = NULL,
        .MSB_ExistingExit = NULL,
        .error = true
    };

    if (!MSB) {
        OSReport("[Rei Wolf]: First we must init MSB!");
        MSB_Init();
    }

    if (id >= MINOR_SCENE_MAX) {
        OSReport("[Rei Wolf]: TOO BIG ID!");
        return;
    }

    MinorSceneDesc_Modified *desc = MSB_GetMinorSceneDescByID(id);
    MSB_Data* saved = MSB_GetFreeSlot();

    if (!desc || !saved) {
        OSReport("[Rei Wolf]: MINOR SCENE DESCRIPTION OR NO SAVED SLOTS?!");
        return;
    }

    *saved = (MSB_Data){
        .think = desc->Think,
        .load = desc->Load,
        .exit = desc->Exit,
        .id = desc->id,
        .taken = true
    };

    callbacks->MSB_ExistingThink = saved->think;
    callbacks->MSB_ExistingLoad = saved->load;
    callbacks->MSB_ExistingExit = saved->exit;
    callbacks->error = false;

    OSReport("[Rei Wolf]: Setting desc->Think (%p) to new_think (%p)", desc->Think, new_think);
    desc->Think = new_think;
    OSReport("[Rei Wolf]: Setting desc->Load (%p) to new_load (%p)", desc->Load, new_load);
    desc->Load = new_load;
    OSReport("[Rei Wolf]: Setting desc->Exit (%p) to new_exit (%p)", desc->Exit, new_exit);
    desc->Exit = new_exit;
}

bool MSB_Restore(u8 id) {
    if (!MSB) return false;

    MSB_Data* saved = MSB_GetSavedByID(id);
    if (!saved) return false;

    MinorSceneDesc_Modified* desc = MSB_GetMinorSceneDescByID(id);
    if (!desc) return false;

    desc->Think = saved->think;
    desc->Load = saved->load;
    desc->Exit = saved->exit;

    *saved = (MSB_Data){
        .think = NULL,
        .load = NULL,
        .exit = NULL,
        .id = 0,
        .taken = false
    };

    if (MSB_SlotsTaken() == 0) {
        MSB_Destroy();
    }

    return true;
}