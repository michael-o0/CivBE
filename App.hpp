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

#define DEBUG

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
#endif /* APP_H */
