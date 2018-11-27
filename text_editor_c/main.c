#include "editor.h"
#include <conio.h>
#include <stdio.h>
int main()
{
	editor_memory ed = editor_create(4,20);
	editor_init_colors(&ed);
	editor_open_file("targetfile.txt", &ed);
	editor_start(&ed);
	editor_print_console_top_ui(&ed);
	editor_print_document(&ed);
	//editor_free(&ed);
	//list_print(&ed.cached_pages);
	getchar();
	return 0;
	//
}