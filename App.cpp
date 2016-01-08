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
#include "GFX.hpp"

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

	App::Options.scale_factor = SCALE_FACTOR_4;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return -1;
	}

	atexit(SDL_Quit);

    //Set the scaling quality to nearest-pixel
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") < 0)
    {
        std::cout << "Failed to set Render Scale Quality" << "\n";
    }

	SDL_Window *window = nullptr;
	window = SDL_CreateWindow("Civ",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		App::Options.base_width * App::Options.scale_factor,
		App::Options.base_height * App::Options.scale_factor,
		SDL_WINDOW_SHOWN
		);
	if (window == nullptr) {
		return -1;
	}

	SDL_Renderer *renderer = nullptr;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer == nullptr) {
		return -1;
	}

	SDL_RenderClear(renderer);

	//Set so it's noticeable if it doesn't come out right.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	//Similarly, you must use SDL_TEXTUREACCESS_TARGET when you create the texture
/*	SDL_Texture *backBuffer = NULL;
	backBuffer = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, 320, 200);
	if (backBuffer == NULL) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		return -1;
	}

	//IMPORTANT Set the back buffer as the target
	SDL_SetRenderTarget(renderer, backBuffer);
*/
	App::Options.SMsCd = "/Volumes/contn/deviceuser/Downloads/Game/civ/";

	std::vector<PICImage> PICs(CivPicFiles.size());
	for (int iter = 0; iter < PICs.size(); ++iter) {
		PICImageIni(App::Options.SMsCd + CivPicFiles[iter], &PICs[iter]);
	}

	std::vector<PICImageTexture> PICImageTextures(PICs.size());
	for (int iter = 0; iter < PICImageTextures.size(); ++iter) {
		GFX::CreatePICImageTexture(&PICs[iter], &PICImageTextures[iter], renderer);
	}

//figure ot ce
/*
Origin of your screen is top/left. 
Middle of the screen:

xcenter = w / 2; ycenter = h/2; 
Left upper point:

x = xcenter - (winrect.width()/2);
y = ycenter - (winrect.height()/2);
Now you only have to place your window there.
*/
//x ((App::Options.base_width * App::Options.scale_factor) / 2) - 
//y ((App::Options.base_height * App::Options.scale_factor) / 2)
// base on rowsize.c
	SDL_RenderClear(renderer);
	SDL_Rect destination = { 0, 0, PICs[0].picimage_256colour.rowsize() * App::Options.scale_factor, PICs[0].picimage_256colour.colsize() * App::Options.scale_factor };
	SDL_RenderCopy(renderer, PICImageTextures[0].PICImageTexture_texture, NULL, &destination);
	SDL_RenderPresent(renderer);

	int tin = 0;

	SDL_Event sdl_event;
	bool done = false;
	while(!done) {
		while(SDL_PollEvent(&sdl_event) != 0) {
			if (sdl_event.type == SDL_QUIT) {
				done = true;
			} else if (sdl_event.type == SDL_KEYDOWN) {
				if (sdl_event.key.keysym.sym == SDLK_RIGHT) {
					if (tin < PICImageTextures.size() - 1) {tin++;} else {tin = 0;};
					if (PICImageTextures[tin].PICImageTexture_texture != nullptr) {
						SDL_RenderClear(renderer);
						SDL_Rect destination = { 0, 0, PICs[tin].picimage_256colour.rowsize() * App::Options.scale_factor, PICs[tin].picimage_256colour.colsize() * App::Options.scale_factor };
						SDL_RenderCopy(renderer, PICImageTextures[tin].PICImageTexture_texture, NULL, &destination);
						SDL_RenderPresent(renderer);
					}
				}
				if (sdl_event.key.keysym.sym == SDLK_LEFT) {
					if (tin == 0) {tin = PICImageTextures.size() - 1;} else {tin--;}
					if (PICImageTextures[tin].PICImageTexture_texture != nullptr) {
						SDL_RenderClear(renderer);
						SDL_Rect destination = { 0, 0, PICs[tin].picimage_256colour.rowsize() * App::Options.scale_factor, PICs[tin].picimage_256colour.colsize() * App::Options.scale_factor };
						SDL_RenderCopy(renderer, PICImageTextures[tin].PICImageTexture_texture, NULL, &destination);
						SDL_RenderPresent(renderer);
					}
				}
			}
		}
	}

	for (int iter = 0; iter < PICs.size(); ++iter) {
		if (PICs[iter].picimage_rawimage != nullptr) {
			delete PICs[iter].picimage_rawimage;
			PICs[iter].picimage_rawimage = nullptr;
		}
	}

	for (int iter = 0; iter < PICImageTextures.size(); ++iter) {
		if (PICImageTextures[iter].PICImageTexture_texture != nullptr) {
			SDL_DestroyTexture(PICImageTextures[iter].PICImageTexture_texture);
			PICImageTextures[iter].PICImageTexture_texture = nullptr;
		}
	}

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;

	std::cout << " Bye.\n";

	return 0;
}
