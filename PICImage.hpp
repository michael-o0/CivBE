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

struct PICImage {
	std::string picimage_keyword;
	std::string picimage_fullfilepath;
	char *picimage_rawimage = nullptr;
	int picimage_rawlength;
	App::Custom::DynamicArray<uint8_t> picimage_colourReplacementTable;
	App::Custom::DynamicArray<uint8_t> picimage_16colour;
	App::Custom::DynamicArray<uint8_t> picimage_256colour;
	SDL_Color picimage_palette16[16];
	SDL_Color picimage_palette256[256];
};

class PICImageIni {
	public:
		PICImageIni(std::string __fullfilepath, struct PICImage *__PICImage) {
			if (FileOpenAndLoad(__fullfilepath, __PICImage) == 0) {
				ReadRawImage(__PICImage);
			}
		}

	private:

		int FileOpenAndLoad(std::string __fullfilepath, struct PICImage *__PICImage) {
			std::ifstream __inpf;

			__inpf.open(__fullfilepath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

			if (!__inpf) {
				std::cerr << "\tPICImage: Unable to open \"" << __fullfilepath  << "\"" << "\n";
				return -1;
			}

			__PICImage->picimage_rawlength = __inpf.tellg();

			__inpf.seekg(0, __inpf.beg);

			__PICImage->picimage_rawimage = new char [ __PICImage->picimage_rawlength ];

			#if defined (DEBUG) || defined(SINGLE_LINE_DEBUG)
			std::cout << "\tPICImage:" << __fullfilepath << ":" << __PICImage->picimage_rawlength << "\n";
			#endif

			if (!__inpf.read(__PICImage->picimage_rawimage, __PICImage->picimage_rawlength)) {
				std::cout << "\tPICImage:" << __fullfilepath << " only " << __inpf.gcount() << " out of " << __PICImage->picimage_rawlength << " could be read. " << "\n";
				delete[] __PICImage->picimage_rawimage;
				__PICImage->picimage_rawimage = nullptr;
				return -1;
			}
			__inpf.close();

			__PICImage->picimage_fullfilepath = __fullfilepath;

			return 0;
		}

		int ReadRawImage(struct PICImage *__PICImage) {

			int __bindex = 0;
			uint16_t __codew;

			while (__bindex < (__PICImage->picimage_rawlength - 1)) {
				__codew = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
				__bindex += 2;
				switch (__codew) {
					case 0x3045:
						ReadE0ColourReplacementTable(__PICImage, __bindex);
						#ifdef SINGLE_LINE_DEBUG
						std::cout << "ReadE0ColourReplacementTable" << "\n";
						#endif
						break;
					case 0x304d:
						ReadM0ColourPalette(__PICImage, __bindex);
						#ifdef SINGLE_LINE_DEBUG
						std::cout << "ReadMColourPalette" << "\n";
						#endif
						break;
					case 0x3058:
						ReadPictureX0(__PICImage, __bindex);
						ConvertPictureX0(__PICImage);
						#ifdef SINGLE_LINE_DEBUG
						std::cout << "Read,ConvertPx0" << "\n";
						#endif
						break;
					case 0x3158:
						ReadPictureX1(__PICImage, __bindex);
						#ifdef SINGLE_LINE_DEBUG
						std::cout << "ReadPictureX1" << "\n";
						#endif
						break;
				}
			}
			return 0;
		}

		/**
		**	Per CivOne <http://www.civone.com/> <https://github.com/SWY1985/CivOne>
		**		Read the E0 colour replacement table from the PIC file.
		**/
		int ReadE0ColourReplacementTable(struct PICImage *__PICImage, int& __bindex) {

			/**
			**	Per CivOne:
			**		// the 4-bit colour conversion table has 2 entries for each colour
			**		// that are painted in a chessboard-pattern, so one 8-bit colour can
			**		// be replaced by two different 4-bit colours
			**/
			App::Custom::DynamicArray<uint8_t> _colour_tbl(256, 2);
			uint16_t _length = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;
			uint8_t _firstIndex = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
			uint8_t _lastIndex = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];

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
				if (i < _firstIndex || i > _lastIndex) {
					for (int j = 0; j < 2; j++) {
						_colour_tbl[i][j] = 0;
					}
					continue;
				}

			/**
			**	Per CivOne:
			**		// split the byte into two nibbles, each containing a colour number
			**/
				_colour_tbl[i][0] = (uint8_t)((__PICImage->picimage_rawimage[ __bindex ] & 0xf0) >> 4);
				_colour_tbl[i][1] = (uint8_t)(__PICImage->picimage_rawimage[ __bindex ] & 0x0f);
//				std::cout << i << " " << std::hex << +(uint8_t)raw[bindex] << " " << +(uint8_t)((raw[bindex] & 0xf0) >> 4) << " " << +(uint8_t)(raw[bindex] & 0x0f) << std::dec << "\n";
//				std::cout << i << " " << std::hex << +(uint8_t)raw[bindex] << " " << +(uint8_t)_colour_tbl[i][0] << " " << +(uint8_t)_colour_tbl[i][1] << std::dec << "\n";
				__bindex++;
			}

			/**
			**	Per CivOne:
			**		// This is a fix for transparency in 16 colour mode
		 	**/
			_colour_tbl[0][0] = 0;
			_colour_tbl[0][1] = 0;

			__PICImage->picimage_colourReplacementTable = _colour_tbl;

			return 0;
		}

		/**
		**	Per CivOne:
		**		Read the M0 colour palette.
		**/
		int ReadM0ColourPalette(struct PICImage *__PICImage, int& __bindex) {
			uint16_t _plen;
			_plen = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);

			__bindex += 2;

			uint8_t _firstIndex = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
			uint8_t _lastIndex = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];

			for (int i = 0; i < 256; i++) {

				/**
				**	Per CivOne:
				**		// if the colour entry fall outside the first/last index range, use
				**		// a transparent colour entry
				**		// this never happens for any of the original Civilization resources
				**/
				if (i < _firstIndex || i > _lastIndex) {
					__PICImage->picimage_palette256[i] = App::Custom::ColorFromArgb(0, 0, 0, 0);
					continue;
				}
				uint8_t _red = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
				uint8_t _green = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
				uint8_t _blue = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
				__PICImage->picimage_palette256[i] = App::Custom::ColorFromArgb(255, _red * 4, _green * 4, _blue * 4);
			}

			/**
			**	Per CivOne:
			**		// always set colour 0 to transparent
			**/
			__PICImage->picimage_palette256[0] = App::Custom::ColorFromArgb(0, 0, 0, 0);

			return 0;
		}

		std::vector<uint8_t> DecodePicture(struct PICImage *__PICImage, int& __bindex, uint32_t __plen) {
			uint8_t _bits = (uint8_t)__PICImage->picimage_rawimage[ __bindex++ ];
			char *ppinraw = __PICImage->picimage_rawimage;
			ppinraw += __bindex;
			std::vector<int> _indeces = App::Custom::LZW::ConvertByteStream(ppinraw, (__plen - 5), _bits);
			std::vector<int> _decoded = App::Custom::LZW::Decode(_indeces);
			std::vector<int> _bytes = App::Custom::RLECodec::Decode(_decoded);

			#ifdef DEBUG
			std::cout << "\tPICImage:" << _indeces.size() << " Indeces\t" << _decoded.size() << " Decoded\t" << _bytes.size() << " Bytes" << "\n";
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

			std::vector<uint8_t> _output;
			for (int i = 0; i < _bytes.size(); i++) {
				_output.push_back((uint8_t)_bytes[i]);
			}

			__bindex += (int)(__plen - 5);
			return _output;
		}

		int ReadPictureX0(struct PICImage *__PICImage, int& __bindex) {
			uint16_t _plen;
			uint16_t _wdth;
			uint16_t _hght;
			App::Custom::DynamicArray<uint8_t> _256colour_picimage;

			_plen = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_wdth = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_hght = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_256colour_picimage.resize( _wdth, _hght);

			std::vector<uint8_t> _image = DecodePicture(__PICImage, __bindex, _plen);

			int c = 0;

			for (int y = 0; y < _hght; y++) {
				for (int x = 0; x < _wdth; x++) {
					_256colour_picimage[x][y] = _image[c++];
				}
			}

			__PICImage->picimage_256colour = _256colour_picimage;

			return 0;
		}

		int ReadPictureX1(struct PICImage *__PICImage, int& __bindex) {
			uint16_t _plen;
			uint16_t _wdth;
			uint16_t _hght;
			App::Custom::DynamicArray<uint8_t> _16colour_picimage;

			_plen = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_wdth = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_hght = App::Custom::BitConvertToUInt16(__PICImage->picimage_rawimage, __bindex);
			__bindex += 2;

			_16colour_picimage.resize(_wdth, _hght);

			std::vector<uint8_t> _image = DecodePicture(__PICImage, __bindex, _plen);

			int c = 0;

			for (int y = 0; y < _hght; y++) {
				for (int x = 0; x < _wdth; x++) {
					_16colour_picimage[ x++ ][ y ] = (uint8_t)(_image[ c ] & 0x0f);
					_16colour_picimage[x][y] = (uint8_t)((_image[ c++ ] & 0xf0) >> 4);
				}
			}

			__PICImage->picimage_16colour = _16colour_picimage;

			return 0;
		}

		int ConvertPictureX0(struct PICImage *__PICImage) {
			if (!__PICImage->picimage_colourReplacementTable.rowsize()) {
				return -1;
			}

			int _wdth;
			int _hght;
			uint8_t _col256;

			_wdth = __PICImage->picimage_256colour.rowsize();
			_hght = __PICImage->picimage_256colour.colsize();

			__PICImage->picimage_16colour.resize(_wdth, _hght);

			for (int y = 0; y < _hght; y++) {
				for (int x = 0; x < _wdth; x++) {
					_col256 = __PICImage->picimage_256colour[x][y];
					__PICImage->picimage_16colour[x][y] = __PICImage->picimage_colourReplacementTable [ _col256 ] [ (y * x) % 2 ];
				}
			}
			return 0;
		}
};

#endif /* PICIMAGE_H */
