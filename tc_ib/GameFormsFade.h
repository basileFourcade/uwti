/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2019 Basile Fourcade
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/
// GAME_FORMS
#ifndef __GAME_FORMS_FADE_H__
#define __GAME_FORMS_FADE_H__

#define MAX_COLOR_SQ_NB		2

color_t color_sequence[ 5 * 5 ][ MAX_COLOR_SQ_NB ] = {
		// game_forms_id 1
		{ NO_COLOR, NO_COLOR },
		{ COLOR_BLUE, COLOR_PURPLE },
		{ COLOR_GREEN, COLOR_INDIGO },
		{ COLOR_RED, COLOR_YELLOW },
		{ NO_COLOR, NO_COLOR },

		// game_forms_id 2
		{ NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, COLOR_INDIGO },
		{ COLOR_RED, COLOR_YELLOW },
		{ COLOR_BLUE, COLOR_PURPLE },
		{ NO_COLOR, NO_COLOR },

		// game_forms_id 3
		{ NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, COLOR_INDIGO },
		{ COLOR_RED, COLOR_YELLOW },
		{ COLOR_BLUE, COLOR_PURPLE },
		{ NO_COLOR, NO_COLOR },

		// game_forms_id 4
		{ NO_COLOR, NO_COLOR },
		{ COLOR_RED, COLOR_YELLOW },
		{ COLOR_GREEN, COLOR_INDIGO },
		{ COLOR_BLUE, COLOR_PURPLE },
		{ NO_COLOR, NO_COLOR },

		// game_forms_id 5
		{ NO_COLOR, NO_COLOR },
		{ COLOR_RED, COLOR_YELLOW },
		{ COLOR_GREEN, COLOR_INDIGO },
		{ NO_COLOR, NO_COLOR },
		{ COLOR_BLUE, COLOR_PURPLE },
		};

#define LWM_SPEED_MS   		20
uint8_t speed_sequence[5] = { LWM_SPEED_MS };

color_t getColorSequenceFromCombinaison(uint8_t combinaison, uint8_t game_forms_id, uint8_t index, uint8_t* fade_speed)
{
	color_t local_color = NO_COLOR;

#ifdef DEBUG_SERIAL
		Serial.println("combinaison " + String(combinaison) +
				" game_forms_id " + String(game_forms_id) + " index " +  String(index));
#endif

	switch (combinaison)
	{
		default:
		case 0x00:
			local_color = color_sequence[(game_forms_id - 1) * 5 + 0][index];
			*fade_speed = speed_sequence[game_forms_id - 1];
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = color_sequence[(game_forms_id - 1) * 5 + 1][index];
			*fade_speed = speed_sequence[game_forms_id - 1];
			break;

			// 2 faces oppos�es
		case 0x0A:
		case 0x05:
			local_color = color_sequence[(game_forms_id - 1) * 5 + 2][index];
			*fade_speed = speed_sequence[game_forms_id - 1];
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = color_sequence[(game_forms_id - 1) * 5 + 3][index];
			*fade_speed = speed_sequence[game_forms_id - 1];
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = color_sequence[(game_forms_id - 1) * 5 + 4][index];
			*fade_speed = speed_sequence[game_forms_id - 1];
			break;
	}

	return local_color;
}

#endif //__GAME_FORMS_FADE_H__
