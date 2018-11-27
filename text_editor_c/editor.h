#pragma once
#include "list.h"
#include <windows.h>

typedef char* text_line;
typedef list_t page; // list of text_line's
typedef struct
{
	//ptr first char on screen

	//ptr last char on screen

	int cache_size; // number of pages to prepare from the file
	int lines_per_page; // number of lines to display on screen at once, only one page on-screen
	page* current_page; // ptr to current page
	list_t cached_pages;// list of current cached pages
	FILE* target_file; // file to edit

	int line_length; // maximum line length

	HANDLE console_handle; // handle to the console for color printing
	
} editor_memory;

editor_memory editor_create(int cache_size);
int editor_open_file(char* filename, editor_memory* editor);
int editor_free(editor_memory* editor);
int editor_start(editor_memory* editor);
int editor_init_colors(editor_memory* editor);
int cprint(editor_memory* ed, int color, const char * format, ...);