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

#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#define PATH_MAX _MAX_PATH
#endif /* _MSC_VER */

#ifdef __WIN32__
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif /* __WIN32__ */

#include "Custom.hpp"
#include "PICImage.hpp"
#include "GFX.hpp"
#include "App.hpp"

int main(int argc, char* args[]) {

	std::cout << "Hello!\n";

	int a = 1;

	while (argc > 1) {
		if (!strcmp(args[a], "s")) {
		++a;
		--argc;
		continue;
		}
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "VIDEONoGO." << std::endl << SDL_GetError() << std::endl;
		return -1;
	}

	atexit(SDL_Quit);

//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "C", "H", NULL);

//	#ifdef DEBUG_SDL
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
//	#endif

	SDL_DisplayMode currentWindow;

//	int numVideoDisplays = SDL_GetNumVideoDisplays();

	if (SDL_GetCurrentDisplayMode(0, &currentWindow) == 0) {
		int _hor_scale_factor;
		int _ver_scale_factor;

		_hor_scale_factor = ((currentWindow.w / GFX::base_width) / 2) * 2;
		std::cout << _hor_scale_factor << std::endl;

		_ver_scale_factor = ((currentWindow.h / GFX::base_height) / 2) * 2;
		std::cout << _ver_scale_factor << std::endl;

		if (_ver_scale_factor < _hor_scale_factor) {
			GFX::scale_factor = _ver_scale_factor;
		} else if (_ver_scale_factor > _hor_scale_factor) {
			GFX::scale_factor = _hor_scale_factor;
		} else {
			GFX::scale_factor = _hor_scale_factor;
		}
	} else {
		SDL_LogMessage(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_WARN, "Could not get CurrentDisplayMode, Going with a GFX scale factor of 2");
		GFX::scale_factor = 2;
	}

	//Set the scaling quality to nearest-pixel
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_WARN, "Could not Set Render Scale Quality");
	}

	SDL_Window *window_AppWindow = nullptr;
	window_AppWindow = SDL_CreateWindow("Civ",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		GFX::base_width * GFX::scale_factor,
		GFX::base_height * GFX::scale_factor,
		SDL_WINDOW_SHOWN
		);
	if (window_AppWindow == nullptr) {
		SDL_LogMessage(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_CRITICAL, "Could not Create AppWindow");
		return -1;
	}

	SDL_Renderer *renderer_Renderer = nullptr;
	renderer_Renderer = SDL_CreateRenderer(window_AppWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer_Renderer == nullptr) {
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_CRITICAL, "Could not Create Rederer");
		return -1;
	}
/*
	SDL_Window *window_TextureRenderWindow = nullptr;
	window_TextureRenderWindow = SDL_CreateWindow("TextureRenderWindow",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		App::Video.window_base_width,// * App::Video.window_scale_factor,
		App::Video.window_base_height,// * App::Video.window_scale_factor,
		SDL_WINDOW_HIDDEN
		);
	if (window_TextureRenderWindow == nullptr) {
		SDL_LogMessage(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_CRITICAL, "Could not Create TextureRenderWindow");
		return -1;
	}

	SDL_Renderer *renderer_TextureRenderer = nullptr;
	renderer_TextureRenderer = SDL_CreateRenderer(window_TextureRenderWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer_TextureRenderer == nullptr) {
		SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_CRITICAL, "Could not Create TextureRenderer");
		return -1;
	}
*/
	SDL_RenderClear(renderer_Renderer);

	//Set so it's noticeable if it doesn't come out right.
	SDL_SetRenderDrawColor(renderer_Renderer, 0, 0, 0, 0);

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

	std::vector<PICImage> PICs(10);//CivPicFiles.size());
	std::vector<PICImageTexture> PICImageTextures(PICs.size());

	for (int iter = 0; iter < PICs.size(); ++iter) {
		PICImageIni(App::Options.SMsCd + CivPicFiles[iter], &PICs[iter]);
	}

	for (int iter = 0; iter < PICImageTextures.size(); ++iter) {
		GFX::Create256ColourPICImageTexture(&PICs[iter], &PICImageTextures[iter], renderer_Renderer);
	}

	int ti = 0;

	GFX::RenderPICImageTexture(PICs, PICImageTextures, ti, renderer_Renderer);

	SDL_Event sdl_event;
	bool quit = false;
	while(!quit) {
		while(SDL_PollEvent(&sdl_event) != 0) {
			switch (sdl_event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch (sdl_event.key.keysym.sym) {
						case SDLK_RIGHT:
							if (ti < PICImageTextures.size() - 1) {ti++;} else {ti = 0;};
							break;
						case SDLK_LEFT:
							if (ti == 0) {ti = PICImageTextures.size() - 1;} else {ti--;}
							break;
					}
					if (PICImageTextures[ti].PICImageTexture_datap != nullptr) {
							GFX::RenderPICImageTexture(PICs, PICImageTextures, ti, renderer_Renderer);
					}
					break;
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
		if (PICImageTextures[iter].PICImageTexture_datap != nullptr) {
			SDL_DestroyTexture(PICImageTextures[iter].PICImageTexture_datap);
			PICImageTextures[iter].PICImageTexture_datap = nullptr;
		}
	}

//	SDL_DestroyRenderer(renderer_TextureRenderer);
//	renderer_TextureRenderer = nullptr;

//	SDL_DestroyWindow(window_TextureRenderWindow);
//	window_TextureRenderWindow = nullptr;


	SDL_DestroyRenderer(renderer_Renderer);
	renderer_Renderer = nullptr;

	SDL_DestroyWindow(window_AppWindow);
	window_AppWindow = nullptr;

	std::cout << " Bye.\n";

	return 0;
}
