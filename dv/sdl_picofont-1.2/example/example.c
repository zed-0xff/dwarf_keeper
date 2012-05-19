/* sdl_picofont

   http://nurd.se/~noname/sdl_picofont

   File authors:
      Fredrik Hultin

   License: GPLv2
*/

#include <SDL.h>
#include <SDL_picofont.h>

int main()
{
	SDL_Surface *screen, *text;
	SDL_Event event;
	SDL_Color color = {255, 255, 255};

	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);

	/* Creates an SDL surface containing the specified text */
	text = FNT_Render("Hello, world!\nPress escape to exit...\n\n\n\tTabbed1\t\t3\n\tTab2\t\t8", color);

	SDL_BlitSurface(text, NULL, screen, NULL);
	SDL_Flip(screen);

	do{
		SDL_PollEvent(&event);
		SDL_WaitEvent(&event);
	}while(event.type != SDL_QUIT && !(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE));

	SDL_FreeSurface(text);

	return 0;
}
