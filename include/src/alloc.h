#ifndef REI_WOLF_ALLOC_H
#define REI_WOLF_ALLOC_H

// Arbitrary, can change as you please
#define INJECT_ALLOC_SIZE 0x8270
#define INJECT_HEAP_ID 6

typedef struct Cell Cell;
typedef struct HeapDesc HeapDesc;

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

void* INJECT_Alloc(size_t size);
void* INJECT_Free(void* ptr);
void INJECT_CreateHeapSpace();
void* INJECT_GetHeapPointer();

#endif // REI_WOLF_ALLOC_H