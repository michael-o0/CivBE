/*

  Copyright (C) 2016 Michael Bur

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <iostream>
#include <fstream>

#include "App.hpp"
#include "PICImage.hpp"

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

int main(int argc, char* args[]) {

	std::cout << "Hello!\n";

	int a = 1;

	while (argc > 1) {
		if (!strcmp(args[a], "Continue")) {
		++a;
		--argc;
		continue;
		}
	}

	PICImage* LOGO = new PICImage("/Volumes/contn/deviceuser/Downloads/Game/civ/SP257.PIC");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	atexit(SDL_Quit);

	SDL_Window* window = nullptr;

	SDL_Surface* wnds = nullptr;
	SDL_Surface* imgs = nullptr;

	SDL_Renderer* renderer = nullptr;

	SDL_CreateWindowAndRenderer(1280, 800, 0, &window, &renderer);

	SDL_RenderClear(renderer);
//	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
/*
	SDL_RenderDrawPoint(renderer, 400, 300); //Renders on middle of screen.

	SDL_RenderPresent(renderer);
*/

//	wnds = SDL_GetWindowSurface(window);

	App::Options.scale_factor = SCALE_FACTOR_4;

	imgs = SDL_CreateRGBSurface(0, 320 * App::Options.scale_factor, 200 * App::Options.scale_factor, 8, 0, 0, 0, 0);
	if (imgs == NULL) {
		std::cerr << "s\n";
		return -1;
	}

	std::cout << std::hex << +imgs->format->BitsPerPixel << " " << +imgs->format->BytesPerPixel << std::dec << "\n";

	if (SDL_MUSTLOCK(imgs)) {
		SDL_LockSurface(imgs);
	}
		Uint8 *p;
	    int bpp = imgs->format->BytesPerPixel;
    	/* Here p is the address to the pixel we want to set */
			for (int y = 0; y < 200; y++) {
				for (int x = 0; x < 320; x++) {
//					std::cout << std::hex << +LOGO->picture256[x][y] << " " << std::dec;
					switch(App::Options.scale_factor) {
						case SCALE_FACTOR_1:
						    p = (Uint8 *)imgs->pixels + y * imgs->pitch + x * bpp;
							*p = LOGO->picture256[x][y];
							break;
						case SCALE_FACTOR_2:
						    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];

							p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch / 2) << "]" << " ";
							break;
						case SCALE_FACTOR_4:
						    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];

							p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp << "]" << " ";
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];

						    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 2) * bpp;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];

						    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 3) * bpp;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];
							p++;
							*p = LOGO->picture256[x][y];

							break;
					}
				}
			}
		SDL_SetPaletteColors(imgs->format->palette, LOGO->palette256, 0, 256);
	if (SDL_MUSTLOCK(imgs)) {
		SDL_UnlockSurface(imgs);
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imgs);
/*
    SDL_Texture *Tile = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, 8, 8);

    // Initialize texture pixels to a red opaque RGBA value
    unsigned char* bytes = nullptr;
    int pitch = 0;
    SDL_LockTexture(Tile, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
    unsigned char rgba[4] = { 255, 0, 0, 255 };
    for(int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            memcpy(&bytes[(y * 8 + x)*sizeof(rgba)], rgba, sizeof(rgba));
        }
    }
    SDL_UnlockTexture(Tile);
*/
	SDL_Rect destination = { 0, 0, 320 * App::Options.scale_factor, 200 * App::Options.scale_factor };
	SDL_RenderCopy(renderer, texture, NULL, &destination);
	SDL_RenderPresent(renderer);

	SDL_Event SDLTypeEvent;
	int notdone = 1;
	while(notdone) {
		while(SDL_PollEvent(&SDLTypeEvent) != 0) {
			switch(SDLTypeEvent.type) {
				case
					SDL_QUIT: notdone = 0;	break;
			}
		}
	}


	SDL_DestroyTexture(texture);

	SDL_FreeSurface(imgs);
//	SDL_FreeSurface(wnds);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);


	delete LOGO;
	LOGO = nullptr;

	std::cout << " Bye.\n";

	return 0;
}
