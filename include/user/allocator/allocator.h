#ifndef REI_WOLF_ALLOC_H
#define REI_WOLF_ALLOC_H

// Arbitrary, can change as you please
#define INJECT_ALLOC_SIZE 0x8270

// Heap ID 6 is the first slot Melee doesn't wipe since it's hard-coded to only wipe 0-5
#define INJECT_HEAP_ID 6

// USB Memory Card Controller extra space (unused by vanilla game)
#define USB_MCC_EXTRA_ADDR 0x8032C998

typedef struct Cell Cell;
typedef struct HeapDesc HeapDesc;
typedef void (*INJECT_Init_Fn)();

typedef struct HeapDesc {
    long  size;
    Cell* free;
    Cell* allocated;
} HeapDesc;

typedef struct Cell {
    Cell* prev;
    Cell* next;
    long size;
} Cell;

typedef struct MajorScene
{
    u8 is_preload;
    s8 major_id;

    u16 unk;

    void (*cb_Load)();
    void (*cb_Exit)();
    void (*cb_Boot)();
    MinorScene *minor_scene_arr;
} MajorScene;

typedef enum INJECT_TYPE {
    INJECT_TYPE_STATIC,
    INJECT_TYPE_DYNAMIC
} INJECT_TYPE;

typedef enum INJECT_SELECTION {
    USB_MCC_EXTRA,
    INJECT_ARENA_LO_HEAP,
    INJECT_SELECTION_ERROR
} INJECT_SELECTION;

typedef struct INJECT_Region {
    void* start;
    size_t size;
    char* name;
    INJECT_Init_Fn init;
    INJECT_TYPE type;
} INJECT_Region;

void* INJECT_Alloc(size_t size);
void INJECT_Free(void* ptr);
void INJECT_CreateHeapSpace();
void INJECT_Prepare(INJECT_SELECTION idx);

#endif // REI_WOLF_ALLOC_H