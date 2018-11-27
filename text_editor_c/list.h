#pragma once
#include <stdio.h>
typedef struct
{
	size_t size;
	void** arr;
	size_t container_size;
}list_t;

list_t list_create(size_t size, size_t container_size);
void list_print(list_t* list);
void list_print_lists(list_t* list);
void* list_get(list_t* list, size_t index);
int list_put(list_t* list, int index, void* val, size_t val_size);
int list_append(list_t* list, void* val, size_t val_size);
int list_free(list_t* list);
void* list_last(list_t* list);