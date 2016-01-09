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

	GFX::Init();

//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "C", "H", NULL);

	App::Options.SMsCd = "/Volumes/contn/deviceuser/Downloads/Game/civ/";

	std::vector<PICImage> PICs(10);//CivPicFiles.size());
	std::vector<PICImageTexture> PICImageTextures(PICs.size());

	for (int iter = 0; iter < PICs.size(); ++iter) {
		PICImageIni(App::Options.SMsCd + CivPicFiles[iter], &PICs[iter]);
	}

	for (int iter = 0; iter < PICImageTextures.size(); ++iter) {
		GFX::Create256ColourPICImageTexture(&PICs[iter], &PICImageTextures[iter], GFX::renderer_GFXRenderer);
	}

	int ti = 0;

	GFX::RenderPICImageTexture(PICs, PICImageTextures, ti, GFX::renderer_GFXRenderer);

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
							GFX::RenderPICImageTexture(PICs, PICImageTextures, ti, GFX::renderer_GFXRenderer);
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

	GFX::Quit();

	std::cout << " Bye.\n";

	return 0;
}
