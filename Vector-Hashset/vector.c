#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void grow(vector *v){
    if(v->log_length == v->alloc_length){
        v->alloc_length *= 2;
        v->elems = realloc(v->elems, v->elem_size * v->alloc_length);
        assert(v->elems != NULL);
    }
}

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    v->alloc_length = initialAllocation;
    if(initialAllocation == 0){
        v->alloc_length = 4;
    }
    v->log_length = 0;
    v->elem_size = elemSize;
    v->elems = malloc(v->alloc_length * elemSize);
    v->free_func = freeFn;
    assert(v->elems != NULL);
}

void VectorDispose(vector *v){
    if(v->free_func != NULL){
        for(int i = 0; i < v->log_length; i++){
            v->free_func((char*)v->elems + i * v->elem_size);
        }
    }
    free(v->elems);
}

int VectorLength(const vector *v){
    return v->log_length;
}

void *VectorNth(const vector *v, int position){
    void* ptr = (char*)v->elems + position * v->elem_size;
    return ptr;
}

void VectorReplace(vector *v, const void *elemAddr, int position){

    void* ptr = (char*)v->elems + position * v->elem_size;
    if(v->free_func != NULL){
        v->free_func(ptr);
    }
    memcpy(ptr, elemAddr, v->elem_size);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(elemAddr != NULL);
    assert (position >= 0);
    assert (position <= v->log_length);
    if(position == v->log_length){
        VectorAppend(v, elemAddr);
        return;
    }
    grow(v);
    void* ptr = (char*)v->elems + position * v->elem_size;
    memmove((char*)ptr + v->elem_size, ptr, (v->log_length - position) * v->elem_size);
    memcpy(ptr, elemAddr, v->elem_size);
    v->log_length++;
}

void VectorAppend(vector *v, const void *elemAddr){
    grow(v);
    void* top = (char*)v->elems + v->log_length * v->elem_size;
    memcpy(top, elemAddr, v->elem_size);
    v->log_length++;
}

void VectorDelete(vector *v, int position){
    assert(v != NULL);
    assert(position >= 0 && position < v->log_length);
    void* ptr = (char*)v->elems + position * v->elem_size;
    if(v->free_func != NULL){
        v->free_func(ptr);
    }
    memmove(ptr, (char*)ptr + v->elem_size, (v->log_length - position - 1) * v->elem_size);
    v->log_length--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    qsort(v->elems, v->log_length, v->elem_size, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    for(int i = 0; i < v->log_length; i++){
        mapFn((char*)v->elems + i * v->elem_size, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){ 
    if(isSorted){
        void* ptr = bsearch(key, v->elems, v->log_length, v->elem_size, searchFn);
        if(ptr != NULL){
            return ((char*)ptr - (char*)v->elems)/v->elem_size;
        }
    } else {
        for(int i = 0; i < v->log_length; i++){
            void* ptr = (char*)v->elems + i * v->elem_size;
            int cur = searchFn(ptr, key);
            if(cur == 0){
                return ((char*)ptr - (char*)v->elems)/v->elem_size;
            }
        }
    }
    return kNotFound;
} 
