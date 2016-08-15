#ifndef _GUTMEM_H_
#define _GUTMEM_H_

void *GutAllocate16BytesAlignedMemory(size_t size);
void GutRelease16BytesAlignedMemory(void *memory);
void *operator new[](size_t size);
void operator delete[](void *p);

#endif // _GUTMEM_H_