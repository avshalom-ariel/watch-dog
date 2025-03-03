/*
Name: Avshalom Ariel
Reveiwer: Or
Date: 01/07/2024
status: Approved
*/

#include <stdlib.h> 	/*malloc free*/
#include <assert.h> 	/*assert*/
#include <sys/types.h>	/*ssize_t*/
#include <stdio.h>

#include "heap.h" 		/*Typedefs, Function declarations*/
#include "dvector.h" 	/*Typedefs, Function declarations*/

#define INIT_CAPACITY 50
#define LEFT(index) 2 * index + 1
#define RIGHT(index) 2 * index + 2
#define PARENT(index) (index - 1) / 2

static void HeapifyUp(heap_t *heap, ssize_t index);
static void HeapifyDown(heap_t *heap, size_t index);

static void Swap(size_t *add1, size_t *add2);

struct heap
{
	vector_t *dvector;
	heap_compare_func_t compare_func;
};


/********************************************/
/*			   API Functions				*/
/********************************************/


heap_t *HeapCreate(heap_compare_func_t compare_func)
{
	heap_t *heap = NULL;
	
	assert(compare_func);

	heap = (heap_t *)malloc(sizeof(heap_t));
	if (!heap)
	{
		return NULL;
	}	
	
	heap->dvector = VectorCreate(INIT_CAPACITY, sizeof(void *));
	if (!heap->dvector)
	{
		free(heap);
		return NULL;
	}
	
	heap->compare_func = compare_func;
	
	return heap;
}


void HeapDestroy(heap_t *heap)
{
	assert(heap);
	
	VectorDestroy(heap->dvector);
	
	free(heap);
}


int HeapPush(heap_t *heap, void *data)
{
	assert(heap);
	
	if (0 != VectorPushBack(heap->dvector, &data))
	{
		return -1;
	}

	HeapifyUp(heap, HeapSize(heap) - 1);
	
	return 0;
}


void HeapPop(heap_t *heap)
{
	assert(heap);

	Swap(VectorAccessVal(heap->dvector, 0), VectorAccessVal(heap->dvector, HeapSize(heap) - 1));
	
	VectorPopBack(heap->dvector);
	
	if (!HeapIsEmpty(heap))
	{
		HeapifyDown(heap, 0);
	}
}


void *HeapPeek(const heap_t *heap)
{
	assert(heap);
	
	return *(void **)VectorAccessVal(heap->dvector, 0);
}


size_t HeapSize(const heap_t *heap)
{
	assert(heap);
	
	return VectorElementCount(heap->dvector);
}


int HeapIsEmpty(const heap_t *heap)
{
	assert(heap);
	
	return 0 == HeapSize(heap);
}


void *HeapRemove(heap_t *heap, heap_match_func_t match_func, void *data_to_match)
{
	size_t index = 0;
	void *data = NULL;
	
	assert(heap);
	assert(match_func);
	
	while (index < HeapSize(heap) && 1 != match_func(data_to_match, 
													*(void **)VectorAccessVal(heap->dvector, index)))
	{
		++index;
	}
	
	if (index == HeapSize(heap))
	{
		return NULL;
	}
	
	data = *(void **)VectorAccessVal(heap->dvector, index);

	Swap(VectorAccessVal(heap->dvector, index), VectorAccessVal(heap->dvector, HeapSize(heap) - 1));
	
	VectorPopBack(heap->dvector);

	if (!HeapIsEmpty(heap))
	{
		HeapifyDown(heap, index);
	}
	
	return data;
}


/********************************************/
/*			   Heapify Functions			*/
/********************************************/


void HeapifyUp(heap_t *heap, ssize_t index)
{
	ssize_t parent = PARENT(index);

	assert(heap);

	if (0 >= index)
	{
		return;
	}
	
	/*if parent is larger, swap, then repeat recursively */
	if(0 < heap->compare_func(*(void **)VectorAccessVal(heap->dvector, index),
							  *(void **)VectorAccessVal(heap->dvector, parent)))
	{
		Swap(VectorAccessVal(heap->dvector, index), VectorAccessVal(heap->dvector, parent));

		HeapifyUp(heap, parent);
	}
}

void HeapifyDown(heap_t *heap, size_t index)
{
	size_t left = LEFT(index);
	size_t right = RIGHT(index);
	size_t largest = index;

	/*check if left child is larger*/
	if (left <=  HeapSize(heap) - 1 && 0 > heap->compare_func(*(void **)VectorAccessVal(heap->dvector, largest),
							  									*(void **)VectorAccessVal(heap->dvector, left)))
	{
		largest = left;
	}

	/*check if right child is larger*/	/*check if left child is larger*/
	if (right <=  HeapSize(heap) - 1 && 0 > heap->compare_func(*(void **)VectorAccessVal(heap->dvector, largest),
																*(void **)VectorAccessVal(heap->dvector, right)))
	{
		largest = right;
	}

	/*swap the larger child (if exist) the repeat recursively*/
	if (index <= ((HeapSize(heap) / 2) - 1) && largest != index)
	{
		Swap(VectorAccessVal(heap->dvector, index), VectorAccessVal(heap->dvector, largest));
		
		HeapifyDown(heap, largest);
	}
}


/********************************************/
/*			   Help Functions				*/
/********************************************/


void Swap(size_t *add1, size_t *add2)
{
	size_t temp = *add1;
	*add1 = *add2;
	*add2 = temp;
}

/*void Print(heap_t *heap)*/
/*{*/
/*	size_t i = 0;*/
/*	*/
/*	for (; i < VectorElementCount(heap->dvector); ++i)*/
/*	{*/
/*		printf("%lu, ", **(int **)VectorAccessVal(heap->dvector, i));*/
/*	}*/
/*	*/
/*	printf("\n");*/
/*}*/


