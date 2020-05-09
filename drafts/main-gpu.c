#include <SFML/Graphics.h>
#include <stdio.h>

int main()
{
	sfEvent *evt;
	sfRenderWindow *window;
	sfVideoMode vm = {800, 600, 32};
	window = sfRenderWindow_create(vm, "CHIP-8", sfClose, NULL);

	sfRectangleShape *re;
	sfVector2f s = {10, 10};
	re = sfRectangleShape_create();
	//sfRectangleShape_setFillColor(re, sfWhite);
	//sfRectangleShape_setSize(re, s);

	while(sfRenderWindow_isOpen(window))
	{
		sfRenderWindow_pollEvent(window, evt);
		if(evt->type == sfEvtClosed) sfRenderWindow_close(window);
		//sfRenderWindow_drawRectangleShape(window, re, NULL);

		sfRenderWindow_clear(window, sfBlack);
		sfRenderWindow_display(window);
	}
}
