#include "p_ed.h"
#include <conio.h>
#include <stdio.h>

int main()
{
	editor_memory ed = editor_create(4,20);
	editor_init_colors(&ed);
	editor_open_file("targetfile.txt", &ed);
	editor_run(&ed);

	//list_print(&ed.cached_pages);
	getchar();
	return 0;
	//
}
//int main()
//{
//	HANDLE  hConsole;
//	int k;
//
//	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//
//	// you can loop k higher to see more color choices
//	for (k = 1; k < 511; k++)
//	{
//		SetConsoleTextAttribute(hConsole, k);
//		printf("%3d  %s\n", k, "I want to be nice today!");
//	}
//
//	getchar();  // wait
//	return 0;
//}