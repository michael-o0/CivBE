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

#ifndef APP_H
#define APP_H

#include <iostream>

#include <istream>
//#include <ostream>

#include <fstream>

#include <streambuf>

#include <vector>

#include <algorithm>

#include <string>

#ifdef _MSC_VER
#define PATH_MAX _MAX_PATH
#endif /* _MSC_VER */

#ifdef __WIN32__
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif /* __WIN32__ */

enum ScaleFactorEnum {
	SCALE_FACTOR_1 = 1,
	SCALE_FACTOR_2 = 2,
	SCALE_FACTOR_4 = 4,
};

namespace App {
	static struct {
		std::string SMsCd;
		const int base_width = 320;
		const int base_height = 200;
		enum ScaleFactorEnum scale_factor;
	} Options;

	namespace Custom {
		template <typename T>
		class DynamicArray {
			public:
				DynamicArray(){};

				DynamicArray(int rows, int cols): dArray(rows, std::vector<T>(cols)){}

				std::vector<T> & operator[](int i) { 
					return dArray[i];
				}
				const std::vector<T> & operator[] (int i) const { 
					return dArray[i];
				}

				int resize(int rows, int cols) {
/*		std::cout << "d " << rows << " " << cols << " " << dArray.size() << "\n";

					if (rows == dArray.size()) {
		std::cout << "r " << rows << " " << cols << " " << dArray.size() << " " << dArray[0].size() << "\n";

						if (cols == dArray[0].size()) {
		std::cout << "c " << rows << " " << cols << " " << dArray.size() << " " << dArray[0].size() << "\n";

							return -1;
						}
					}
		std::cout << "a " << rows << " " << cols << " " << dArray.size() << "\n";

					if (dArray.size() < rows) {
*/						dArray.resize(rows);
						for(int i = 0;i < rows;++i) dArray[i].resize(cols);
/*					} else {
						for(int i = 0;i < rows;++i) dArray[i].resize(cols);
						dArray.resize(rows);
					}
		std::cout << "f " << rows << " " << cols << " " << dArray.size() << " " << dArray[0].size() << "\n";
*/
					return 0;
				}
				int rowsize() {
					return dArray.size();
				}
				int colsize() {
					return dArray[0].size();
				}
			private:
				std::vector<std::vector<T> > dArray;  
		};

		static uint16_t BitConvertToUInt16(char *buf, int p) {
			uint16_t r;
			r = (uint8_t)buf[p+1];
			r <<= 8;
			r |= (uint8_t)buf[p];
			return r;
		}

		static SDL_Color ColorFromArgb(uint8_t _a, uint8_t _r, uint8_t _g, uint8_t _b) {
			SDL_Color c = {_r, _g, _b, _a};
			return c;
		}

		namespace LZW {
			class LZWDictionary {
				public:
					typedef struct {
						std::vector<int> e;
					} LZWDictionaryEntry;

					LZWDictionary() {
						int dictionarySize = (1 << MAX_BITS);

						for (int i = 0; i < dictionarySize; i++) {
							if (i < 256) {
								_dictionary[i][0] = i;
							} else {
								_dictionary[i][0] = -1;								
							}
						}

						_cursorPosition = 257;
					}

					int AddEntry(std::vector<int> entry) {
						if (_cursorPosition < _dictionary.size()) {
							_dictionary[_cursorPosition++] = entry;
						}
						return _cursorPosition - 1;
					}

					std::vector<int> GetEntry(int position) {
						if (position < _cursorPosition) {
							return _dictionary[position];
						}
						return ev;
					}

					int CursorPosition() {
						return _cursorPosition;
					}

					bool IsFull() {
						if (_cursorPosition == _dictionary.size()) {
							return true;
						}
						return false;
					}

				private:
					const int MAX_BITS = 11;
					//App::Custom::DynamicArray<int> _dictionary;
					//std::vector<std::vector<int> > _dictionary((1 << MAX_BITS), std::vector<int>(nullptr));
					std::vector<std::vector<int> > _dictionary = std::vector<std::vector<int> >(2048, std::vector<int>(1));
					std::vector<int> ev;
					//std::map<int, int> _dictionary;
					int _cursorPosition;
			};

			static std::vector<int> Decode(std::vector<int> input) {
				std::vector<int> output;

				while (input.size() > 0) {

					LZWDictionary* dic = new LZWDictionary();
					int prev = input[0];
					std::vector<int> buffer = dic->GetEntry(prev);

					std::vector<int> data;
					data.push_back(prev);

					int lastChar = prev;

					int i = 0;
					while (!dic->IsFull() && i < input.size() - 1) {
						std::vector<int> entry;
						if (input[++i] < dic->CursorPosition()) {
							entry = dic->GetEntry(input[i]);
						} else {
							entry = dic->GetEntry(prev);
							entry.push_back(lastChar);
						}

						std::copy(entry.begin(), entry.end(), std::back_inserter(data));

						lastChar = entry[0];

						std::vector<int> newEntry;
						std::copy(buffer.begin(), buffer.end(), std::back_inserter(newEntry));
						newEntry.push_back(lastChar);
						dic->AddEntry(newEntry);

						prev = input[i];

						buffer = dic->GetEntry(prev);
					}

					std::copy(data.begin(), data.end(), std::back_inserter(output));

					std::vector<int>::const_iterator frm = input.begin() + (i + 1);
					std::vector<int>::const_iterator to = input.end();
					std::vector<int> tmp(frm, to);
					input = tmp;
					delete dic;
					dic = nullptr;
				}
				return output;
			}

			static std::vector<int> ConvertByteStream(char *buf, int remainingCodedBytes, int bits) {
				std::vector<int> parsedIndexes;

				int usableBits = 0;
				int usableBitCount = 0;

				// Directly from CivOne
				int indicatorLength = 1; // to increment with ++; rule is that 8+indicatorLength must be <= bits, otherwise reset
				int indicatorFlag = 0x001; // to increment with <<=1 followed by |= 1
				int nextThreshold = 0x0100; // to increment with <<=1, or *=2 - 256
				int decodedCounter = 0;
				int index = 0;


				while (remainingCodedBytes > 0)
				{
					/* get enough coded bits to work with */
					while (usableBitCount < 8 + indicatorLength)
					{
						usableBits |= ((uint8_t)*buf++ << usableBitCount);
						remainingCodedBytes--;
						usableBitCount += 8;
					}

					/* decode bytes and indicators */
					while (usableBitCount >= 8 + indicatorLength)
					{
						index = usableBits & (((indicatorFlag << 8) & 0xFF00) | 0x00FF);

						//int decodedByte = usableBits & 0xFF;
						usableBits >>= 8;
						usableBitCount -= 8;

						//int decodedIndicator = usableBits & indicatorFlag;
						usableBits >>= indicatorLength;
						usableBitCount -= indicatorLength;

						decodedCounter++;

						if (decodedCounter == nextThreshold)
						{
							decodedCounter = 0;
							indicatorLength += 1;
							indicatorFlag <<= 1;
							indicatorFlag |= 1;
							nextThreshold <<= 1;

							if (8 + indicatorLength > bits)
							{
								decodedCounter = 0;
								indicatorLength = 1;
								indicatorFlag = 0x001;
								nextThreshold = 256;
							}
						}
						parsedIndexes.push_back(index);
					}
				}

				return parsedIndexes;
			}
		}

		namespace RLECodec {
			static std::vector<int> Decode(std::vector<int> input) {
				const int RLE_REPEAT = 0x90;

				if (input.empty()) {
					return input;
				}

				std::vector<int> output;
				output.push_back(input[0]);

				for (int i = 1; i < input.size(); i++) {

					if (input[i] != RLE_REPEAT || input[i + 1] == 0) {
						output.push_back(input[i]);
						if (input[i] == RLE_REPEAT && input[i + 1] == 0) i++;
						continue;
					}

					int repeat = input[i + 1];
					int start = output.size();
					int value = output[output.size() - 1];

					int rbl = output.size() + (repeat - 1);
					for (int j = start; j < rbl; j++) {
						output.push_back(value);
					}

					i++;
				}
				return output;
			}
		}
	}
}

char const* CivPicFilesNames[] = {
		"ADSCREEN.PIC",
		"ARCH.PIC",
		//"ASOUND.CVL",
		//"BACK0A.PAL",
		"BACK0A.PIC",
		//"BACK0M.PAL",
		"BACK0M.PIC",
		//"BACK1A.PAL",
		"BACK1A.PIC",
		//"BACK1M.PAL",
		"BACK1M.PIC",
		//"BACK2A.PAL",
		"BACK2A.PIC",
		//"BACK2M.PAL",
		"BACK2M.PIC",
		//"BACK3A.PAL",
		"BACK3A.PIC",
		//"BIRTH0.PAL",
		"BIRTH0.PIC",
		//"BIRTH1.PAL",
		"BIRTH1.PIC",
		//"BIRTH2.PAL",
		"BIRTH2.PIC",
		//"BIRTH3.PAL",
		"BIRTH3.PIC",
		//"BIRTH4.PAL",
		"BIRTH4.PIC",
		//"BIRTH5.PAL",
		"BIRTH5.PIC",
		//"BIRTH6.PAL",
		"BIRTH6.PIC",
		//"BIRTH7.PAL",
		"BIRTH7.PIC",
		//"BIRTH8.PAL",
		"BIRTH8.PIC",
		//"BLURB0.TXT",
		//"BLURB1.TXT",
		//"BLURB2.TXT",
		//"BLURB3.TXT",
		//"BLURB4.TXT",
		"CASTLE0.PIC",
		"CASTLE1.PIC",
		"CASTLE2.PIC",
		"CASTLE3.PIC",
		"CASTLE4.PIC",
		"CBACK.PIC",
		"CBACKS1.PIC",
		"CBACKS2.PIC",
		"CBACKS3.PIC",
		"CBRUSH0.PIC",
		"CBRUSH1.PIC",
		"CBRUSH2.PIC",
		"CBRUSH3.PIC",
		"CBRUSH4.PIC",
		"CBRUSH5.PIC",
		"CITYPIX1.PIC",
		"CITYPIX2.PIC",
		"CITYPIX3.PIC",
		//"CIV.EXE",
		//"CREDITS.TXT",
		"CUSTOM.PIC",
		"DIFFS.PIC",
		//"DISCOVR1.PAL",
		"DISCOVR1.PIC",
		//"DISCOVR2.PAL",
		"DISCOVR2.PIC",
		"DOCKER.PIC",
		//"EGRAPHIC.EXE",
		//"ERROR.TXT",
		//"FONTS.CV",
		"GOVT0A.PIC",
		"GOVT0M.PIC",
		"GOVT1A.PIC",
		"GOVT1M.PIC",
		"GOVT2A.PIC",
		"GOVT2M.PIC",
		"GOVT3A.PIC",
		//"HELP.TXT",
		//"HILL.PAL",
		"HILL.PIC",
		//"ICONPG1.PAL",
		"ICONPG1.PIC",
		"ICONPG2.PIC",
		"ICONPG3.PIC",
		"ICONPG4.PIC",
		"ICONPG5.PIC",
		"ICONPG6.PIC",
		"ICONPG7.PIC",
		"ICONPG8.PIC",
		//"ICONPGA.PAL",
		"ICONPGA.PIC",
		"ICONPGB.PIC",
		"ICONPGC.PIC",
		"ICONPGD.PIC",
		"ICONPGE.PIC",
		"ICONPGT1.PIC",
		"ICONPGT2.PIC",
		//"INTRO.TXT",
		//"INTRO3.TXT",
		"INVADER2.PIC",
		"INVADER3.PIC",
		"INVADERS.PIC",
		//"ISOUND.CVL",
		//"KING.TXT",
		//"KING00.PAL",
		"KING00.PIC",
		//"KING01.PAL",
		"KING01.PIC",
		//"KING02.PAL",
		"KING02.PIC",
		//"KING03.PAL",
		"KING03.PIC",
		//"KING04.PAL",
		"KING04.PIC",
		//"KING05.PAL",
		"KING05.PIC",
		//"KING06.PAL",
		"KING06.PIC",
		//"KING07.PAL",
		"KING07.PIC",
		//"KING08.PAL",
		"KING08.PIC",
		//"KING09.PAL",
		"KING09.PIC",
		//"KING10.PAL",
		"KING10.PIC",
		//"KING11.PAL",
		"KING11.PIC",
		//"KING12.PAL",
		"KING12.PIC",
		//"KING13.PAL",
		"KING13.PIC",
		"KINK00.PIC",
		"KINK03.PIC",
		"LOGO.PIC",
		"LOVE1.PIC",
		"LOVE2.PIC",
		"MAP.PIC",
		//"MGRAPHIC.EXE",
		//"MISC.EXE",
		//"NSOUND.CVL",
		"NUKE1.PIC",
		"PLANET1.PIC",
		"PLANET2.PIC",
		"POP.PIC",
		//"PRODUCE.TXT",
		//"READ.ME",
		"RIOT.PIC",
		"RIOT2.PIC",
		//"RSOUND.CVL",
		"SAD.PIC",
		"SETTLERS.PIC",
		"SLAG2.PIC",
		//"SLAM1.PAL",
		"SLAM1.PIC",
		"SLAM2.PIC",
		//"SP256.PAL",
		//"SP257.PAL",
		"SP257.PIC",
		"SP299.PIC",
		"SPACEST.PIC",
		"SPRITES.PIC",
		//"STORY.TXT",
		"TER257.PIC",
		//"TGRAPHIC.EXE",
		//"TORCH.PIC",
		//"TSOUND.CVL",
		"WONDERS.PIC",
		"WONDERS2.PIC"
/*
"LOGO.PIC",
"SP257.PIC",
"SP299.PIC",
"LOVE1.PIC",
"LOVE2.PIC",
//"MAP.PIC",
"ARCH.PIC",
"TER257.PIC",
"WONDERS.PIC",
"WONDERS2.PIC",
"BACK0A.PIC",
"BACK0M.PIC",
"BACK1A.PIC",
"BACK1M.PIC",
"BACK2A.PIC",
"BACK2M.PIC",
"BACK3A.PIC",
"BIRTH0.PIC",
"BIRTH1.PIC",
"BIRTH2.PIC",
"BIRTH3.PIC",
"BIRTH4.PIC",
"BIRTH5.PIC",
"BIRTH6.PIC",
"BIRTH7.PIC",
"BIRTH8.PIC",
"CASTLE0.PIC",
"CASTLE1.PIC",
"CASTLE2.PIC",
"CASTLE3.PIC",
"CASTLE4.PIC",
"CBACK.PIC",
"CBACKS.PIC",
"CBACKS1.PIC",
"CBACKS2.PIC",
"CBACKS3.PIC",
//"CBRUSH0.PIC",
//"CBRUSH1.PIC",
//"CBRUSH2.PIC",
//"CBRUSH3.PIC",
//"CBRUSH4.PIC",
//"CBRUSH5.PIC",
"CITYPIX1.PIC",
"CITYPIX2.PIC",
"CITYPIX3.PIC",
"CUSTOM.PIC",
"DIFFS.PIC",
"DISCOVR1.PIC",
"DISCOVR2.PIC",
"DOCKER.PIC",
"GOVT0A.PIC",
"GOVT0M.PIC",
"GOVT1A.PIC",
"GOVT1M.PIC",
"GOVT2A.PIC",
"GOVT2M.PIC",
"GOVT3A.PIC",
"HILL.PIC",
"ICONPG1.PIC",
"ICONPG2.PIC",
"ICONPG3.PIC",
"ICONPG4.PIC",
"ICONPG5.PIC",
"ICONPG6.PIC",
"ICONPG7.PIC",
"ICONPG8.PIC",
"ICONPGA.PIC",
"ICONPGB.PIC",
"ICONPGC.PIC",
"ICONPGD.PIC",
"ICONPGE.PIC",
"ICONPGT1.PIC",
"ICONPGT2.PIC",
"INVADER2.PIC",
"INVADER3.PIC",
"INVADERS.PIC",
"KING00.PIC",
"KING01.PIC",
"KING02.PIC",
"KING03.PIC",
"KING04.PIC",
"KING05.PIC",
"KING06.PIC",
"KING07.PIC",
"KING08.PIC",
"KING09.PIC",
"KING10.PIC",
"KING11.PIC",
"KING12.PIC",
"KING13.PIC",
//"KINK00.PIC",
//"KINK03.PIC",
"NUKE1.PIC",
//"PLANET1.PIC",
"PLANET2.PIC",
"POP.PIC",
"RIOT.PIC",
"RIOT2.PIC",
"SAD.PIC",
"SETTLERS.PIC",
//"SLAG2.PIC",
//"SLAM1.PIC",
//"SLAM2.PIC",
//"SPACEST.PIC",
//"SPRITES.PIC",
*/
};

template <typename T, int Size>
T* begin(T (&array)[Size]) {
    return array;
}
template <typename T, int Size>
T* end(T (&array)[Size]) {
    return array + Size;
}

std::vector<std::string> CivPicFiles(begin(CivPicFilesNames), end(CivPicFilesNames));

#endif /* APP_H */
