/*

  Copyright (C) 2016 Michael Burge

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

#ifndef GFX_H
#define GFX_H

#ifdef	__WIN32__
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif	/* __WIN32__ */

struct PICImageTexture {
	std::string PICImageTexture_keyword;
	SDL_Texture *PICImageTexture_texture = nullptr;
	int PICImageTexture_width;
	int PICImageTexture_height;
};

namespace GFX {
	int CreatePICImageTexture(struct PICImage *__PICImage, struct PICImageTexture *__PICImageTexture, SDL_Renderer *renderer) {
		SDL_Surface* imgs = nullptr;

		imgs = SDL_CreateRGBSurface(0, 320 * App::Options.scale_factor, 200 * App::Options.scale_factor, 8, 0, 0, 0, 0);
		if (imgs == NULL) {
			std::cerr << "s\n";
			return -1;
		}

		if (SDL_MUSTLOCK(imgs)) {
			SDL_LockSurface(imgs);
		}
			Uint8 *p;
	    	int bpp = imgs->format->BytesPerPixel;
				for (int y = 0; y < 200; y++) {
					for (int x = 0; x < 320; x++) {
//						std::cout << std::hex << +__PICImage->picimage_256colour[x][y] << " " << std::dec;
						switch(App::Options.scale_factor) {
							case SCALE_FACTOR_1:
							    p = (Uint8 *)imgs->pixels + y * imgs->pitch + x * bpp;
								*p = __PICImage->picimage_256colour[x][y];
								break;
							case SCALE_FACTOR_2:
							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];

								p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch / 2) << "]" << " ";
								break;
							case SCALE_FACTOR_4:
							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];

								p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp << "]" << " ";
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];

							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 2) * bpp;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];

							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 3) * bpp;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];
								p++;
								*p = __PICImage->picimage_256colour[x][y];

								break;
						}
					}
				}

		SDL_SetPaletteColors(imgs->format->palette, __PICImage->picimage_palette256, 0, 256);

		if (SDL_MUSTLOCK(imgs)) {
			SDL_UnlockSurface(imgs);
		}

		__PICImageTexture->PICImageTexture_texture = SDL_CreateTextureFromSurface(renderer, imgs);

		SDL_FreeSurface(imgs);
		imgs = nullptr;

		return 0;
	}
}


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

#endif /* GFX_H */
