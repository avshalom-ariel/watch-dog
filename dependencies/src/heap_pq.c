/*
Name: Avshalom Ariel
Reveiwer: Or
Date: 01/07/2024
status: Approved
*/

#include <stdlib.h> 	/*malloc, free*/
#include <assert.h> 	/*assert*/

#include "heap.h" 		/*typedefs, function declarations*/
#include "heap_pq.h" 	/*typedefs, function declarations*/

struct heap_pq
{
	heap_t *heap;
};


/********************************************/
/*			   API Functions				*/
/********************************************/


heap_pq_t *HeapPQCreate(heap_pqcompare_func_t cmp_func)
{
	heap_pq_t *heap_pq = (heap_pq_t *)malloc(sizeof(heap_pq_t));
	if(!heap_pq)
	{
		return NULL;
	}
	
	heap_pq->heap = HeapCreate(cmp_func);
	if(!heap_pq->heap)
	{
		free(heap_pq);
		return NULL;
	}
	
	return heap_pq;
}

void HeapPQDestroy(heap_pq_t *heap_pq)
{
	assert(heap_pq);
	
	HeapDestroy(heap_pq->heap);
	free(heap_pq);
}

int HeapPQEnqueue(heap_pq_t *heap_pq, void *data)
{
	assert(heap_pq);
	
	return HeapPush(heap_pq->heap, data);
}

void *HeapPQDequeue(heap_pq_t *heap_pq)
{
	void *data = NULL;

	assert(heap_pq);
	
	data = HeapPeek(heap_pq->heap);
	
	HeapPop(heap_pq->heap);
	
	return data;
}

void *HeapPQPeek(const heap_pq_t *heap_pq)
{
	assert(heap_pq);
	
	return HeapPeek(heap_pq->heap);
}

int HeapPQIsEmpty(const heap_pq_t *heap_pq)
{
	assert(heap_pq);
	
	return HeapIsEmpty(heap_pq->heap);
}

size_t HeapPQSize(const heap_pq_t *heap_pq)
{
	assert(heap_pq);
	
	return HeapSize(heap_pq->heap);
}

void HeapPQClear(heap_pq_t *heap_pq)
{
	assert(heap_pq);
	
	while(!HeapPQIsEmpty(heap_pq))
	{
		HeapPQDequeue(heap_pq);
	}
}

void *HeapPQErase(heap_pq_t *heap_pq, heap_pqmatch_func_t is_match, void *data)
{	
	assert(heap_pq);
	
	return HeapRemove(heap_pq->heap, is_match, data);
}


