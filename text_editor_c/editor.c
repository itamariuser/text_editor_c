#include "editor.h"
#include <conio.h>

/**
D: Init a new editor without opening any files*/
editor_memory editor_create(int cache_size)
{
	editor_memory ret_editor = { cache_size, 10, NULL, NULL, NULL };
	ret_editor.cached_pages = list_create(0, cache_size);
	ret_editor.line_length = 128;
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
