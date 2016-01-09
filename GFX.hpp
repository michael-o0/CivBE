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

#ifndef GFX_HPP
#define GFX_HPP

#ifdef	__WIN32__
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif	/* __WIN32__ */

struct PICImageTexture {
	std::string PICImageTexture_id;
	int PICImageTexture_width;
	int PICImageTexture_height;
	SDL_Texture *PICImageTexture_datap = nullptr;
};

namespace GFX {
	const int base_width = 320;
	const int base_height = 200;
	int scale_factor;

	int RenderPICImageTexture(std::vector<PICImage> PICs, std::vector<PICImageTexture> PICImageTextures, int ti, SDL_Renderer *renderer_Renderer) {

		int renderer_Renderer_w;
		int renderer_Renderer_h;

		SDL_RenderClear(renderer_Renderer);
		SDL_GetRendererOutputSize(renderer_Renderer, &renderer_Renderer_w, &renderer_Renderer_h);
		SDL_Rect source = { 0, 0, PICs[ti].picimage_256colour.rowsize(), PICs[ti].picimage_256colour.colsize() };
		SDL_Rect destination = { 0, 0, renderer_Renderer_w, renderer_Renderer_h };
		SDL_RenderCopy(renderer_Renderer, PICImageTextures[ti].PICImageTexture_datap, &source , &destination);
		SDL_RenderPresent(renderer_Renderer);

		return 0;
	}

	int Create16ColourPICImageTexture(struct PICImage *__PICImage, struct PICImageTexture *__PICImageTexture, SDL_Renderer *renderer_Renderer) {
		SDL_Surface* _surface_Imgsrfc = nullptr;

		if ((__PICImage->picimage_16colour.rowsize() == 0) || (__PICImage->picimage_16colour.colsize() == 0)) {
			std::cerr << "\tPICImageTexture\t\"" << __PICImage->picimage_fullfilepath << "\"\t" << "NULL" << "\n";
			return -1;
		}
std::cout << __PICImage->picimage_fullfilepath << " " << __PICImage->picimage_16colour.rowsize() << " " << __PICImage->picimage_16colour.colsize() << std::endl;
		_surface_Imgsrfc = SDL_CreateRGBSurface(0,
			__PICImage->picimage_16colour.rowsize(),
			__PICImage->picimage_16colour.colsize(),
			8,
			0,
			0,
			0,
			0
			);
		if (_surface_Imgsrfc == NULL) {
			std::cerr << "s\n";
			return -1;
		}

		if (SDL_MUSTLOCK(_surface_Imgsrfc)) {
			SDL_LockSurface(_surface_Imgsrfc);
		}
			Uint8 *p;
	    	int bpp = _surface_Imgsrfc->format->BytesPerPixel;
				for (int y = 0; y < __PICImage->picimage_16colour.colsize(); y++) {
					for (int x = 0; x < __PICImage->picimage_16colour.rowsize(); x++) {
//						std::cout << std::hex << +__PICImage->picimage_16colour[x][y] << " " << std::dec;
//						switch(App::Options.scale_factor) {
//							case SCALE_FACTOR_1:
							    p = (Uint8 *)_surface_Imgsrfc->pixels + y * _surface_Imgsrfc->pitch + x * bpp;
								*p = __PICImage->picimage_16colour[x][y];
/*								break;
							case SCALE_FACTOR_2:
							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];

								p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch / 2) << "]" << " ";
								break;
							case SCALE_FACTOR_4:
							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];

								p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp;
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor << "]";
//std::cout << "[" << y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch) * bpp << "]" << " ";
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];

							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 2) * bpp;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];

							    p = (Uint8 *)imgs->pixels + y * imgs->pitch * App::Options.scale_factor + x * bpp * App::Options.scale_factor + (imgs->pitch * 3) * bpp;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];
								p++;
								*p = __PICImage->picimage_16colour[x][y];

								break;
						}*/
					}
				}

		SDL_SetPaletteColors(_surface_Imgsrfc->format->palette, App::Custom::standard16Colours, 0, 16);

		if (SDL_MUSTLOCK(_surface_Imgsrfc)) {
			SDL_UnlockSurface(_surface_Imgsrfc);
		}

		__PICImageTexture->PICImageTexture_datap = SDL_CreateTextureFromSurface(renderer_Renderer, _surface_Imgsrfc);
		__PICImageTexture->PICImageTexture_width = __PICImage->picimage_16colour.rowsize();
		__PICImageTexture->PICImageTexture_height = __PICImage->picimage_16colour.colsize();

		SDL_FreeSurface(_surface_Imgsrfc);
		_surface_Imgsrfc = nullptr;

		return 0;
	}

	int Create256ColourPICImageTexture(struct PICImage *__PICImage, struct PICImageTexture *__PICImageTexture, SDL_Renderer *renderer_Renderer) {
		SDL_Surface* _surface_Imgsrfc = nullptr;

		if ((__PICImage->picimage_256colour.rowsize() == 0) || (__PICImage->picimage_256colour.colsize() == 0)) {
			std::cerr << "\tPICImageTexture\t\"" << __PICImage->picimage_fullfilepath << "\"\t" << "NULL" << "\n";
			return -1;
		}

		_surface_Imgsrfc = SDL_CreateRGBSurface(0,
			__PICImage->picimage_256colour.rowsize(),
			__PICImage->picimage_256colour.colsize(),
			8,
			0,
			0,
			0,
			0
			);
		if (_surface_Imgsrfc == NULL) {
			std::cerr << "s\n";
			return -1;
		}

		if (SDL_MUSTLOCK(_surface_Imgsrfc)) {
			SDL_LockSurface(_surface_Imgsrfc);
		}
			Uint8 *p;
	    	int bpp = _surface_Imgsrfc->format->BytesPerPixel;
				for (int y = 0; y < __PICImage->picimage_256colour.colsize(); y++) {
					for (int x = 0; x < __PICImage->picimage_256colour.rowsize(); x++) {
//						std::cout << std::hex << +__PICImage->picimage_256colour[x][y] << " " << std::dec;
//						switch(App::Options.scale_factor) {
//							case SCALE_FACTOR_1:
							    p = (Uint8 *)_surface_Imgsrfc->pixels + y * _surface_Imgsrfc->pitch + x * bpp;
								*p = __PICImage->picimage_256colour[x][y];
/*								break;
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
						}*/
					}
				}

		SDL_SetPaletteColors(_surface_Imgsrfc->format->palette, __PICImage->picimage_palette256, 0, 256);

		if (SDL_MUSTLOCK(_surface_Imgsrfc)) {
			SDL_UnlockSurface(_surface_Imgsrfc);
		}

		__PICImageTexture->PICImageTexture_datap = SDL_CreateTextureFromSurface(renderer_Renderer, _surface_Imgsrfc);
		__PICImageTexture->PICImageTexture_width = __PICImage->picimage_256colour.rowsize();
		__PICImageTexture->PICImageTexture_height = __PICImage->picimage_256colour.colsize();

		SDL_FreeSurface(_surface_Imgsrfc);
		_surface_Imgsrfc = nullptr;

		return 0;
	}
}

#endif /* !GFX_HPP */
