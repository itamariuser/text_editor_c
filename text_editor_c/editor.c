#include "editor.h"
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
	ret_editor.selec_len = 0;
	return ret_editor;
}

/**
D: Open a the target file for editing
R: success: editor->target_file | failure: -1*/
int editor_open_file(char* filename, editor_memory* editor)
{
	if ( NULL == (editor->target_file = fopen(filename, "r+")))
	{
		perror("couldn't open file, error: ");
		return -1;
	}
	editor->target_name = filename;
	return editor->target_file;
}

/**
D: Free resources
R: success: 0 | failure: -1*/
int editor_free(editor_memory* editor)
{
	if (fclose(editor->target_file) || list_free(&editor->cached_pages))
	{
		perror("failed freeing editor: ");
		return -1;
	}
	return 0;
}

/**
D: Open the ui for editing
R: success : 0 | failure : -1*/
int editor_start(editor_memory * editor)
{
	text_line read_line = (char*)malloc(editor->line_length * sizeof(char));
	for (int page_num = 0; page_num < editor->cache_size; ++page_num)
	{
		//printf("page #%d:\n", page_num);
		// create a new page and populate it
		page new_page = list_create(0, editor->lines_per_page);
		for (int line_num = 0; 
			read_line &&
			line_num < editor->lines_per_page;
			++line_num)
		{
			if (!fgets(read_line, editor->line_length, editor->target_file))
				break;
			//printf("read line #%d: %s\n", line_num, read_line);
			// append the dynamic read_line to editor page
			list_append(&new_page, read_line, strlen(read_line)+1);
		}
		printf("PAGE #%d: ",page_num);
		list_print(&new_page);

		//push the populated page into editor's pages
		list_append(&editor->cached_pages, &new_page, sizeof(new_page));// TODO?: change back to +(sizeof(char*)*new_page.container_size));
		//printf("*****ALL PAGES: *****\n");
		//list_print_lists(&editor->cached_pages);
		//printf("\n");

		// init some editor data
		editor->current_page = editor->cached_pages.arr[0];
		editor->cursor_pos = editor->current_page->arr[0]; // cursor on first char
	}

	free(read_line);
	return 0;
}

int editor_init_colors(editor_memory * editor)
{
	if (INVALID_HANDLE_VALUE == (editor->console_handle = GetStdHandle(STD_OUTPUT_HANDLE)))
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
int cprint(editor_memory* ed, int color, const char * format, ...)
{
	static CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	static WORD saved_attributes;
	// save prev console state
	GetConsoleScreenBufferInfo(ed->console_handle, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	// print with variable args
	SetConsoleTextAttribute(ed->console_handle, color);
	va_list args;
	va_start(args, format);
	int result = vprintf(format, args);

	// restore prev console state
	SetConsoleTextAttribute(ed->console_handle, saved_attributes);
	return result;


}

int editor_print_console_top_ui(editor_memory * ed)
{
	// warning: implementation depends on OS
	clrscreen();

	// display asterisk if unsaved
	char* unsaved_asterisk = ed->unsaved_changes ? "*" : "";
	//print top line 
	cprint(ed, 122, "Itamar Sheffer C Editor v0.1            File: %s%s\n", ed->target_name, unsaved_asterisk);
	return 0;
}


int editor_print_document(editor_memory* ed)
{
	// print current page and current line separately for easy editing
	// TODO: print current line with current position marked in gray
	// TODO: check current page and read new pages from file if needed
	// TODO: check if printing selected chars, if yes -> use cprint(110)
	cprint(ed, 110, "%c", *ed->cursor_pos); // TEST
	for(size_t line_num = 0; line_num < ed->lines_per_page; ++line_num)
	{
		text_line curr_line = ed->current_page->arr[line_num];
		if (!curr_line)
			break;
		printf("%s\n", curr_line);
	}
	return 0;
}