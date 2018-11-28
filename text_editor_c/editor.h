#pragma once
#include "list.h"
#include <windows.h>

typedef struct  
{
	int page_index; // pos within the page
	int line_index; // pos within the line
	char* p_val; // char in the position
} text_pos;

typedef struct
{
	char* start;
	size_t len;
} text_line;
typedef list_t page; // list of text_line's
typedef struct
{

	//ptr last char on screen?

	int cache_size; // number of pages to prepare from the file
	int lines_per_page; // number of lines to display on screen at once, only one page on-screen
	page* current_page; // ptr to current page
	list_t cached_pages;// list of current cached pages
	FILE* target_file; // file to edit

	int line_length; // maximum line length
	char* target_name;
	HANDLE h_console_output; // handle to the console for color printing
	HANDLE h_console_input; // handle to the console for color printing
	BYTE unsaved_changes; // TRUE if there are unsaved changes, FALSE otherwise

	char* cursor_pos; // marks start of selection
	BYTE selec_mode; // TRUE if selecting, FALSE otherwise
	text_pos selec_start;
	text_pos selec_end;

	WORD text_color;
	WORD selec_color;
	
} editor_memory;


editor_memory editor_create(int cache_size, int lines_per_page);
int editor_open_file(char* filename, editor_memory* editor);
int editor_populate_from_file(editor_memory* editor);
int editor_free(editor_memory* p_ed);
int editor_run(editor_memory* ed);


int editor_init_colors(editor_memory* editor);
int editor_print_console_top_ui(editor_memory* ed);

int cprint(editor_memory* ed, int color, const char * format, ...);


// position operations
/**
INCOMPLETE, USE WITH CAUTION
*/
text_pos pos_inc(text_pos* p, editor_memory* ed);
/**
INCOMPLETE, USE WITH CAUTION
*/
text_pos pos_dec(text_pos* p, editor_memory* ed);