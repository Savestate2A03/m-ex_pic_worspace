#include "system.h"
#include "alloc.h"

extern HeapDesc* stc_ptr_to_OSHeapArray;
extern int stc_max_number_of_heaps;
static u8* injection_heap_pointer = NULL;

void* INJECT_Alloc(size_t size) {
    int current_heap_id = HSD_GetHeapID();
    HSD_SetHeapID(INJECT_HEAP_ID);
    void* ptr = HSD_MemAlloc(size);
    HSD_SetHeapID(current_heap_id);
    return ptr;
}

void* INJECT_Free(void* ptr) {
    int current_heap_id = HSD_GetHeapID();
    HSD_SetHeapID(INJECT_HEAP_ID);
    HSD_Free(ptr);
    HSD_SetHeapID(current_heap_id);
}

void* INJECT_GetHeapPointer() {
    return &injection_heap_pointer;
}

void INJECT_CreateHeapSpace() {

    Cell* cell;
    HeapDesc* heap;

    void* start = injection_heap_pointer;
    void* end = injection_heap_pointer + INJECT_ALLOC_SIZE;

    heap = &(stc_ptr_to_OSHeapArray[INJECT_HEAP_ID]); 

    heap->size = (u32)end - (u32)start;
    cell = start;
    cell->prev = NULL;
    cell->next = NULL;
    cell->size = heap->size;
    heap->free = cell;
    heap->allocated = NULL;

    int starting_heap_id = HSD_GetHeapID();
    HSD_SetHeapID(INJECT_HEAP_ID);

    OSReport("[Rei Wolf]: Injected Heap: %p - %p\n", start, start + INJECT_ALLOC_SIZE);
    OSReport("[Rei Wolf]: Injected Heap ID: %d\n", INJECT_HEAP_ID);

    HSD_SetHeapID(starting_heap_id);
}