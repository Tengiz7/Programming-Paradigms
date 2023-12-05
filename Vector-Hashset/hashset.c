#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets, HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	h->elem_size = elemSize;
	h->num_buckets = numBuckets;
	h->free_fn = freefn;
	h->hash_cmp = comparefn;
	h->hash_fn = hashfn;
	h->n_elements = 0;
	h->elems = malloc(numBuckets * sizeof (vector));
	assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);
	for(int i = 0; i < numBuckets; i++){
		vector* vec = (vector*)h->elems + i;
		VectorNew(vec, elemSize, freefn, 4);
	}
}

void HashSetDispose(hashset *h){
	for(int i = 0; i < h->num_buckets; i++){
		vector* vec = (vector*)h->elems + i;
		VectorDispose(vec);
	}
	free(h->elems);
}

int HashSetCount(const hashset *h){
	return h->n_elements;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	for(int i = 0; i < h->num_buckets; i++){
		//printf("%d\n", i);
		vector* vec = (vector*)h->elems + i;
		VectorMap(vec, mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	int pos = h->hash_fn(elemAddr, h->num_buckets);
	vector* vec = (vector*)h->elems + pos;
	int cur = VectorSearch(vec, elemAddr, h->hash_cmp, 0, 0);
	if(cur == -1) {
		//printf("%c\n", *(char*)elemAddr);
		VectorAppend(vec, elemAddr);
		h->n_elements++;
	} else {
		VectorReplace(vec, elemAddr, cur);
	} 
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	int pos = h->hash_fn(elemAddr, h->num_buckets);
	vector* ptr = (vector*)h->elems + pos;
	int cur = VectorSearch(ptr, elemAddr, h->hash_cmp, 0, 0);
	if(cur != -1){
		return (char*)ptr->elems + cur * ptr->elem_size;
	} 
	return NULL;
}
