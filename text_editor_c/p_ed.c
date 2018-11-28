#include "p_ed.h"
#ifdef _WIN32
#define clrscreen() system("cls")
#else
#include <stdio.h>
#define clrscreen() printf("\e[1;1H\e[2J")
#endif

/**
D: Init a new editor without opening any files*/
editor_memory editor_create(int cache_size, int lines_per_page)
{
	if (lines_per_page <= 0)
		lines_per_page = 10;
	editor_memory ret_editor = { cache_size, lines_per_page, NULL, NULL, NULL };
	ret_editor.cached_pages = list_create(0, cache_size);
	ret_editor.line_length = 128;
	ret_editor.unsaved_changes = FALSE;
	ret_editor.selec_mode = FALSE; // start with just cursor, not selecting
	memset(&ret_editor.selec_start, 0, sizeof(text_pos));
	memset(&ret_editor.selec_end, 0, sizeof(text_pos));
	ret_editor.text_color = 7;
	ret_editor.selec_color = 112;
	return ret_editor;
}

/**
D: Open a the target file for editing
R: success: editor->target_file | failure: -1*/
int editor_open_file(char* filename, editor_memory* p_ed)
{
	if ( NULL == (p_ed->target_file = fopen(filename, "r+")))
	{
		perror("couldn't open file, error: ");
		return -1;
	}
	p_ed->target_name = filename;
	return editor_populate_from_file(p_ed);
}

/**
D: Free resources
R: success: 0 | failure: -1*/
int editor_free(editor_memory* p_ed)
{
	if (fclose(p_ed->target_file) || list_free(&p_ed->cached_pages))
	{
		perror("failed freeing editor: ");
		return -1;
	}
	return 0;
}


int editor_populate_from_file(editor_memory * p_ed)
{
	text_line read_line = (char*)malloc(p_ed->line_length * sizeof(char));
	for (int page_num = 0; page_num < p_ed->cache_size; ++page_num)
	{
		//printf("page #%d:\n", page_num);
		// create a new page and populate it
		page new_page = list_create(0, p_ed->lines_per_page);
		for (int line_num = 0; 
			read_line &&
			line_num < p_ed->lines_per_page;
			++line_num)
		{
			if (!fgets(read_line, p_ed->line_length, p_ed->target_file))
				break;
			//printf("read line #%d: %s\n", line_num, read_line);
			// append the dynamic read_line to editor page
			list_append(&new_page, read_line, strlen(read_line)+1);
		}
		printf("PAGE #%d: ",page_num);
		list_print(&new_page);

		//push the populated page into editor's pages
		list_append(&p_ed->cached_pages, &new_page, sizeof(new_page));// TODO?: change back to +(sizeof(char*)*new_page.container_size));
		//printf("*****ALL PAGES: *****\n");
		//list_print_lists(&editor->cached_pages);
		//printf("\n");

		// init some editor data
		p_ed->current_page = p_ed->cached_pages.arr[0];
		p_ed->cursor_pos = p_ed->current_page->arr[0]; // cursor on first char
		p_ed->selec_start.p_val = p_ed->cursor_pos;
		p_ed->selec_end.p_val = p_ed->cursor_pos;
	}

	free(read_line);
	return 0;
}

int editor_init_colors(editor_memory * p_ed)
{
	if (INVALID_HANDLE_VALUE == (p_ed->console_handle = GetStdHandle(STD_OUTPUT_HANDLE)))
	{
		perror("couldn't init console colors: ");
		return -1;
	}
	return 0;
}

/**
D: Like printf but with COLOR
R: *See vprintf return values*
USE: cprint(&ed, 128 | BACKGROUND_INTENSITY, "eyy %s", "lmao\n");
*/
int cprint(editor_memory* p_ed, int color, const char * format, ...)
{
	static CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	static WORD saved_attributes;
	// save prev console state
	GetConsoleScreenBufferInfo(p_ed->console_handle, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	// print with variable args
	SetConsoleTextAttribute(p_ed->console_handle, color);
	va_list args;
	va_start(args, format);
	int result = vprintf(format, args);

	// restore prev console state
	SetConsoleTextAttribute(p_ed->console_handle, saved_attributes);
	return result;


}

text_pos pos_inc(text_pos * p, editor_memory * p_ed)
{
	p->p_val++;
	// TODO: CHECK IF OUT OF BOUNDS AND UPDATE PAGE/LINE
	return *p;
}

text_pos pos_dec(text_pos * p, editor_memory * p_ed)
{
	p->p_val--;
	// TODO: CHECK IF OUT OF BOUNDS AND UPDATE PAGE/LINE
	return *p;
}

/**
D: Open the ui for editing
R: success : 0 | failure : -1*/
int editor_run(editor_memory* p_ed)
{
	BYTE should_exit = FALSE;
	// main loop
	while (!should_exit)
	{
		clrscreen(); // warning: implementation depends on OS
		editor_print_console_top_ui(p_ed);
		editor_print_document(p_ed);
		pos_inc(&p_ed->selec_start, p_ed);
		Sleep(100);
	}

	// free resources and prepare for exit
	editor_free(&p_ed);
}

int editor_print_console_top_ui(editor_memory * p_ed)
{
	// display asterisk if unsaved
	char* unsaved_asterisk = p_ed->unsaved_changes ? "*" : "";
	//print top line 
	return cprint(p_ed, 122, "Itamar Sheffer C Editor v0.1            File: %s%s\n", p_ed->target_name, unsaved_asterisk);
}


int editor_print_document(editor_memory* p_ed)
{
	// print current page and current line separately for easy editing
	// TODO: print current line with current position marked in gray
	// TODO: check current page and read new pages from file if needed
	// TODO: check if printing selected chars, if yes -> use cprint(selec_color)
	cprint(p_ed, p_ed->selec_color, "%c", *p_ed->selec_start.p_val); // TEST
	for(size_t line_num = 0; line_num < p_ed->lines_per_page; ++line_num)
	{
		text_line curr_line = p_ed->current_page->arr[line_num];
		if (!curr_line)
			break;
		cprint(p_ed, p_ed->text_color,"%s\n", curr_line);
	}
	return 0;
}