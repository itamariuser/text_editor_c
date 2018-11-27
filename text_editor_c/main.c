#include "editor.h"

int main()
{
	editor_memory ed = editor_create(4);
	editor_init_colors(&ed);
	editor_open_file("targetfile.txt", &ed);
	editor_start(&ed);
	cprint(&ed, 128 | BACKGROUND_INTENSITY, "eyy %s", "lmao\n");
	//editor_free(&ed);
	//list_print(&ed.cached_pages);
	getchar();
	return 0;
}