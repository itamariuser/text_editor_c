#include "list.h"
#include <string.h>
list_t list_create(size_t size, size_t container_size)
{
	if (container_size < size)
		container_size = size;

	//void** arr;
	//auto zoob = (void**)calloc(container_size * sizeof(void*));
	void** azz = calloc(container_size, sizeof(void*));
	/*if (NULL == (arr = (void**)calloc(container_size * sizeof(void*)))) // WHY DOES IT ALLOCATE 100MB?!?*/
	//if (!zoob)
	//	perror("dynamic allocation failed: ");
	
	list_t ret_list = { size, azz, container_size };
	return ret_list;
}

void list_print(list_t* list)
{
	for (size_t i = 0; i < list->size; ++i)
	{
		printf("%s, ", list->arr[i]);
	}
}

/* if the list contains items of type list_t, print them correctly
USE ONLY ON A LIST OF list_t's*/
void list_print_lists(list_t* list) 
{
	for (size_t i = 0; i < list->size; ++i)
	{
		printf("\nitem #%d: \n",i);
		list_print(list->arr[i]);
	}
}

void* list_get(list_t * list, size_t index)
{
	if (index < list->size)
		return list->arr[index];
	perror("out of bounds\n");
}

int list_put(list_t * list, int index, void* val, size_t val_size)
{
	if (index < list->size)
	{
		list->arr[index] = (void*)malloc(sizeof(void*) * val_size);
		//add to list
		memcpy(list->arr[index], val, val_size);
		return val;
	}
	perror("out of bounds");
}

int list_append(list_t * list, void* val, size_t val_size)
{
	if (!val)
	{
		val = calloc(sizeof(int));
	}
	if (list->size == list->container_size)
	{
		//reallocations
		//double list size, if its 0, make new size = 1
		int new_size = list->container_size == 0 ? 1 : list->container_size * 2;
		if (NULL == (list->arr = (char**)realloc(list->arr, new_size * sizeof(char*))))// if realloc failed
		{
			perror("realloc error: ");
			return -1;
		}
		list->container_size = new_size;
	}
	//allocate enough memory for the new val
	list->arr[list->size] = (void*)malloc(sizeof(void*) * val_size);
	//add to list
	if (NULL == memcpy(list->arr[list->size], val, val_size))
	{
		perror("MEMCOPY FAILED: ");
		return 1;
	}
	/*list->arr[list->size] = val;*/
	list->size++;
	return val;
}

int list_free(list_t* list)
{
	for (size_t i = 0; i < list->size; ++i)
	{
		free(list->arr[i]);
	}
	free(list->arr);
}

// get address of last element of the list
void * list_last(list_t * list)
{
	return &list->arr[list->size - 1];
}
