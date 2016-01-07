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

#ifndef PICIMAGE_H
#define PICIMAGE_H

#include "App.hpp"

class PICImage {

	public:

		PICImage() {};

		PICImage(std::string res) {
			LoadRaw(res);
		}

/*		int Load(std::string res) {
			PICImage::~PICImage();
			LoadRaw(res);
			return 0;
		}
*/
		char *raw = nullptr;
		long len;

		~PICImage() {
			if (raw != nullptr) {
				delete[] raw;
				raw = nullptr;
			}
		}

//	private:

		int LoadRaw(std::string res) {

			std::ifstream inpf;

			inpf.open(res.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
			if (!inpf) {
				std::cerr << "\tPICImage: Unable to open \"" << res  << "\"" << "\n";
				return -1;
			}
			//inpf.seekg(0, inpf.end);
			len = inpf.tellg();
			inpf.seekg(0, inpf.beg);

			raw = new char [len];

			#ifdef DEBUG
			std::cout << "\tPICImage:" << res << ":" << len << "\n";
			#endif

			if (!inpf.read(raw, len)) {
				std::cout << "\tPICImage:" << res << " only " << inpf.gcount() << " out of " << len << " could be read. " << "\n";
				delete[] raw;
				return -1;
			}
			inpf.close();

			int bindex = 0;
			uint16_t codew;

			while (bindex < (len - 1)) {
				codew = App::Custom::BitConvertToUInt16(raw, bindex);
				bindex += 2;
				switch (codew) {
					case 0x3045:
						colour_tbl = ReadColourTable(bindex);
//						std::cout << "ReadColourTable" << "\n";
						break;
					case 0x304d:
						ReadColourPalette(bindex);
//						std::cout << "ReadColourPalette" << "\n";
						break;
					case 0x3058:
						ReadPictureX0(bindex);
						ConvertPictureX0(colour_tbl);
//						std::cout << "Read,ConvertPx0" << "\n";
						break;
					case 0x3158:
						ReadPictureX1(bindex);
//						std::cout << "ReadPictureX1" << "\n";
						break;
				}
			}
			return 0;
		}

		/**
		**	Per CivOne <http://www.civone.com/> <https://github.com/SWY1985/CivOne>
		**		Read the E0 colour replacement table from the PIC file.
		**/
		App::Custom::DynamicArray<uint8_t> ReadColourTable(int& bindex) {

			/**
			**	Per CivOne:
			**		// the 4-bit colour conversion table has 2 entries for each colour
			**		// that are painted in a chessboard-pattern, so one 8-bit colour can
			**		// be replaced by two different 4-bit colours
			**/
			App::Custom::DynamicArray<uint8_t> _colour_tbl(256, 2);
			uint16_t length = App::Custom::BitConvertToUInt16(raw, bindex);
			bindex += 2;
			uint8_t firstIndex = (uint8_t)raw[bindex++];
			uint8_t lastIndex = (uint8_t)raw[bindex++];

//std::cout << "c " << std::hex << +length << " " << +firstIndex << " " << +lastIndex << std::dec << "\n";

			//std::cout << std::hex << (uint16_t)firstIndex << " " << (uint16_t)lastIndex << std::dec << std::endl;

			/**
			**	Per CivOne:
			**		// create all colour entries
			**/
			for (int i = 0; i < 256; i++) {

				/**
				**	Per CivOne:
				**		// if the colour entries fall outside the first/last index range, they
				**		// will use colour 0 (transparent)
				**		// this never happens for any of the original Civilization resources
				**/
				if (i < firstIndex || i > lastIndex) {
					for (int j = 0; j < 2; j++) {
						_colour_tbl[i][j] = 0;
					}
					continue;
				}

			/**
			**	Per CivOne:
			**		// split the byte into two nibbles, each containing a colour number
			**/
				_colour_tbl[i][0] = (uint8_t)((raw[bindex] & 0xf0) >> 4);
				_colour_tbl[i][1] = (uint8_t)(raw[bindex] & 0x0f);
//				std::cout << i << " " << std::hex << +(uint8_t)raw[bindex] << " " << +(uint8_t)((raw[bindex] & 0xf0) >> 4) << " " << +(uint8_t)(raw[bindex] & 0x0f) << std::dec << "\n";
//				std::cout << i << " " << std::hex << +(uint8_t)raw[bindex] << " " << +(uint8_t)_colour_tbl[i][0] << " " << +(uint8_t)_colour_tbl[i][1] << std::dec << "\n";
				bindex++;
			}

			/**
			**	Per CivOne:
			**		// This is a fix for transparency in 16 colour mode
		 	**/
			_colour_tbl[0][0] = 0;
			_colour_tbl[0][1] = 0;
			return _colour_tbl;
		}

		/**
		**	Per CivOne:
		**		Read the M0 colour palette.
		**/
		int ReadColourPalette(int& bindex) {
			uint16_t plen;

			plen = App::Custom::BitConvertToUInt16(raw, bindex);
			bindex += 2;

			uint8_t firstIndex = (uint8_t)raw[bindex++];
			uint8_t lastIndex = (uint8_t)raw[bindex++];

			for (int i = 0; i < 256; i++) {

				/**
				**	Per CivOne:
				**		// if the colour entry fall outside the first/last index range, use
				**		// a transparent colour entry
				**		// this never happens for any of the original Civilization resources
				**/
				if (i < firstIndex || i > lastIndex) {
					palette256[i] = App::Custom::ColorFromArgb(0, 0, 0, 0);
					continue;
				}
				uint8_t red = (uint8_t)raw[bindex++];
				uint8_t green = (uint8_t)raw[bindex++];
				uint8_t blue = (uint8_t)raw[bindex++];
				palette256[i] = App::Custom::ColorFromArgb(255, red * 4, green * 4, blue * 4);
			}

			/**
			**	Per CivOne:
			**		// always set colour 0 to transparent
			**/
			palette256[0] = App::Custom::ColorFromArgb(0, 0, 0, 0);
			return 0;
		}

		std::vector<unsigned char> DecodePicture(int& bindex, uint32_t plen) {
			uint8_t bits = (uint8_t)raw[bindex++];
			char *ppinraw = raw;
			ppinraw += bindex;
			std::vector<int> indeces = App::Custom::LZW::ConvertByteStream(ppinraw, (plen - 5), bits);
			std::vector<int> decoded = App::Custom::LZW::Decode(indeces);
			std::vector<int> bytes = App::Custom::RLECodec::Decode(decoded);

			#ifdef DEBUG
			std::cout << "\tPICImage:" << indeces.size() << " Indeces\t" << decoded.size() << " Decoded\t" << bytes.size() << " Bytes" << "\n";
			#endif

//			for (std::vector<int>::iterator it = indeces.begin() ; it != indeces.end(); ++it)
//				std::cout << ' ' << *it;
//			std::cout << '\n';

//			for (std::vector<int>::iterator it = decoded.begin() ; it != decoded.end(); ++it)
//				std::cout << ' ' << *it;
//			std::cout << '\n';

//			for (std::vector<int>::iterator it = bytes.begin() ; it != bytes.end(); ++it)
//				std::cout << ' ' << *it;
//			std::cout << '\n';

			std::vector<uint8_t> output;
			for (int i = 0; i < bytes.size(); i++) {
				output.push_back((uint8_t)bytes[i]);
			}

			bindex += (int)(plen - 5);
			return output;
		}

		int ReadPictureX0(int& bindex) {
			uint16_t plen = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;
			uint16_t wdth = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;
			uint16_t hght = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;

			picture256.resize(wdth, hght);

			std::vector<uint8_t> image = DecodePicture(bindex, plen);

			int c = 0;

			for (int y = 0; y < hght; y++) {
				for (int x = 0; x < wdth; x++) {
					picture256[x][y] = image[c++];
				}
			}

			return 0;
		}

		int ReadPictureX1(int& bindex) {
			uint16_t plen = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;
			uint16_t wdth = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;
			uint16_t hght = App::Custom::BitConvertToUInt16(raw, bindex); bindex += 2;

			picture16.resize(wdth, hght);

			std::vector<uint8_t> image = DecodePicture(bindex, plen);

			int c = 0;

			for (int y = 0; y < hght; y++) {
				for (int x = 0; x < wdth; x++) {
					picture16[x++][y] = (uint8_t)(image[c] & 0x0f);
					picture16[x][y] = (uint8_t)((image[c++] & 0xf0) >> 4);
				}
			}

			return 0;
		}

		int ConvertPictureX0(App::Custom::DynamicArray<uint8_t> clrtbl) {
			if (!clrtbl.rowsize()) {
				return -1;
			}

			int wdth = picture256.rowsize();
			int hght = picture256.colsize();

			picture16.resize(wdth, hght);

			for (int y = 0; y < hght; y++) {
				for (int x = 0; x < wdth; x++) {
					uint8_t col256 = picture256[x][y];
					picture16[x][y] = clrtbl[col256][(y*x)%2];
				}
			}
			return 0;
		}

		App::Custom::DynamicArray<uint8_t> colour_tbl;
		App::Custom::DynamicArray<uint8_t> picture16;
		App::Custom::DynamicArray<uint8_t> picture256;
		SDL_Color palette16[16];
		SDL_Color palette256[256];
};

#endif /* PICIMAGE_H */
