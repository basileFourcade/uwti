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
#ifndef __GAME_FORMS_H__
#define __GAME_FORMS_H__

#define DIFFICULTY_EASY		0
#define DIFFICULTY_HARD		1
#define NUMBER_RULES		5

color_t rulesEasy[5 * NUMBER_RULES][NUMBER_PIXELS] =
{
// game_forms_id 1
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_BLUE, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, NO_COLOR, COLOR_GREEN, NO_COLOR },
		{ COLOR_RED, COLOR_RED, NO_COLOR, NO_COLOR },
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },

		// game_forms_id 2
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_RED, NO_COLOR, COLOR_RED, NO_COLOR },
		{ COLOR_BLUE, COLOR_BLUE, NO_COLOR, NO_COLOR },
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },

		// game_forms_id 3
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_RED, NO_COLOR, COLOR_RED, NO_COLOR },
		{ COLOR_BLUE, COLOR_BLUE, NO_COLOR, NO_COLOR },
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },

		// game_forms_id 4
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_RED, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, NO_COLOR, COLOR_GREEN, NO_COLOR },
		{ COLOR_BLUE, COLOR_BLUE, NO_COLOR, NO_COLOR },
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },

		// game_forms_id 5
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_RED, NO_COLOR, NO_COLOR, NO_COLOR },
		{ COLOR_GREEN, NO_COLOR, COLOR_GREEN, NO_COLOR },
		{ COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, NO_COLOR },
		{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR }, };

color_t gameFormsIdShow[5 + 1][NUMBER_PIXELS] =
{
{ NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
{ GAME_FORMS_ID_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
{ GAME_FORMS_ID_COLOR, GAME_FORMS_ID_COLOR, NO_COLOR, NO_COLOR },
{ GAME_FORMS_ID_COLOR, NO_COLOR, GAME_FORMS_ID_COLOR, NO_COLOR },
{ GAME_FORMS_ID_COLOR, GAME_FORMS_ID_COLOR, GAME_FORMS_ID_COLOR, NO_COLOR },
{ GAME_FORMS_ID_COLOR, GAME_FORMS_ID_COLOR, GAME_FORMS_ID_COLOR,
		GAME_FORMS_ID_COLOR } };

uint8_t rules_idx_temp = 0;

// rules_idx from 0 to 4
void showRules(uint8_t game_forms_id, uint8_t rules_idx)
{
	uint8_t local_idx = ((game_forms_id - 1) * NUMBER_RULES) + rules_idx;

#ifdef DEBUG_SERIAL
	Serial.println("game_forms_id = " + String(game_forms_id));
	Serial.println("rules_idx = " + String(rules_idx));
	Serial.println("local_idx = " + String(local_idx));
#endif

	setLedringFourPixels(rulesEasy[local_idx][0], rulesEasy[local_idx][1],
			rulesEasy[local_idx][2], rulesEasy[local_idx][3],
			VALUE_COLOR_MEDIUM);

}

void showNextNonEmptyRules(uint8_t game_forms_id)
{
	uint8_t local_idx;

	do
	{
		/* Try next then */
		rules_idx_temp++;
		rules_idx_temp = rules_idx_temp % NUMBER_RULES;
		local_idx = ((game_forms_id - 1) * NUMBER_RULES) + rules_idx_temp;
	} while ((rulesEasy[local_idx][0] == 0) && (rulesEasy[local_idx][1] == 0)
			&& (rulesEasy[local_idx][2] == 0) && (rulesEasy[local_idx][3] == 0));

	showRules(game_forms_id, rules_idx_temp);
}

void resetShowNextRules(void)
{
	rules_idx_temp = 0;
}

void showNextRules(uint8_t game_forms_id)
{
	/* Try next then */
	rules_idx_temp++;
	/* But skip the last one */
	rules_idx_temp = rules_idx_temp % (NUMBER_RULES - 1);

	showRules(game_forms_id, rules_idx_temp);
}

void showRulesWithColor(uint8_t game_forms_id, color_t color)
{
	uint8_t local_idx = ((game_forms_id - 1) * NUMBER_RULES);

#ifdef DEBUG_SERIAL
	/* */
	Serial.println("local_idx = " + String(local_idx));
#endif
	/* Look up rules associated with color */
	for (uint8_t idx = local_idx; idx < (local_idx + NUMBER_RULES); idx++)
	{
#ifdef DEBUG_SERIAL
		/* */
		Serial.println("idx = " + String(idx));
#endif
		if (rulesEasy[idx][0] == color)
		{
#ifdef DEBUG_SERIAL
			/* */
			Serial.println("color found at idx = " + String(idx));
#endif
			setLedringFourPixels(rulesEasy[idx][0], rulesEasy[idx][1],
					rulesEasy[idx][2], rulesEasy[idx][3], VALUE_COLOR_MEDIUM);
			break;
		}
	}
}

void showGameFormsId(uint8_t game_forms_id, uint8_t speed_ms, uint8_t loop,
		boolean dont_clear)
{
	uint8_t i; // Avoid compiler warning and overflow

	if (loop > 0)
	{
		for (i = 1; i <= loop; i++)
		{
			setLedringFourPixels(gameFormsIdShow[game_forms_id][0],
					gameFormsIdShow[game_forms_id][1],
					gameFormsIdShow[game_forms_id][2],
					gameFormsIdShow[game_forms_id][3], VALUE_COLOR_MEDIUM);
			delay(speed_ms);
			setLedringRGB(0, 0, 0);
			delay(speed_ms);
		}
	}

	if (loop == 0 || dont_clear)
	{
		setLedringFourPixels(gameFormsIdShow[game_forms_id][0],
				gameFormsIdShow[game_forms_id][1],
				gameFormsIdShow[game_forms_id][2],
				gameFormsIdShow[game_forms_id][3], VALUE_COLOR_MEDIUM);
	}
}

void showCombiWithColor(uint8_t combi_id, color_t color, uint8_t speed_ms,
		uint8_t loop, uint8_t keepLast)
{
	color_t colorLed[4] =
	{ NO_COLOR };

	for (uint8_t idx = 0; idx < 4; idx++)
	{
		if (gameFormsIdShow[combi_id][idx] != NO_COLOR)
		{
			colorLed[idx] = color;
		}
	}

	if (loop > 0)
	{
		for (uint8_t i = 1; i <= loop; i++)
		{
			setLedringFourPixels(colorLed[0], colorLed[1], colorLed[2],
					colorLed[3], VALUE_COLOR_MEDIUM);
			delay(speed_ms);
			setLedringRGB(0, 0, 0);
			delay(speed_ms);
		}
		if (keepLast)
		{
			setLedringFourPixels(colorLed[0], colorLed[1], colorLed[2],
					colorLed[3], VALUE_COLOR_MEDIUM);
		}
	}
	else
	{
		setLedringFourPixels(colorLed[0], colorLed[1], colorLed[2], colorLed[3],
				VALUE_COLOR_MEDIUM);
	}
}

color_t getColorFromCombinaisonEasy(uint8_t combinaison, uint8_t game_forms_id)
{
	color_t local_color = NO_COLOR;

	if (game_forms_id == 1)
	{
		switch (combinaison)
		{
		default:
		case 0x00:
			local_color = NO_COLOR;
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = COLOR_BLUE;
			break;

			// 2 faces opposï¿½es
		case 0x0A:
		case 0x05:
			local_color = COLOR_GREEN;
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = COLOR_RED;
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 2)
	{
		switch (combinaison)
		{
		default:
		case 0x00:
			local_color = NO_COLOR;
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = COLOR_GREEN;
			break;

			// 2 faces opposï¿½es
		case 0x0A:
		case 0x05:
			local_color = COLOR_RED;
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = COLOR_BLUE;
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 3)
	{
		switch (combinaison)
		{
		default:
		case 0x00:
			local_color = NO_COLOR;
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = COLOR_GREEN;
			break;

			// 2 faces opposées
		case 0x0A:
		case 0x05:
			local_color = COLOR_RED;
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = COLOR_BLUE;
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 4)
	{
		switch (combinaison)
		{
		default:
		case 0x00:
			local_color = NO_COLOR;
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = COLOR_RED;
			break;

			// 2 faces opposées
		case 0x0A:
		case 0x05:
			local_color = COLOR_GREEN;
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = COLOR_BLUE;
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;

		}
	}
	else if (game_forms_id == 5)
	{
		switch (combinaison)
		{
		default:
		case 0x00:
			local_color = NO_COLOR;
			break;

			// 1 face
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
			local_color = COLOR_RED;
			break;

			// 2 faces opposées
		case 0x0A:
		case 0x05:
			local_color = COLOR_GREEN;
			break;

			// 2 faces adjacentes
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x09:
			local_color = COLOR_WHITE;
			break;

			// 3 faces
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_BLUE;
			break;
		}
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("Unknown game_forms_id!");
#endif
	}

	return local_color;
}

color_t getColorFromCombinaisonHard(uint8_t combinaison, uint8_t game_forms_id)
{
	color_t local_color = NO_COLOR;

	if (game_forms_id == 1)
	{
		switch (combinaison)
		{
		case 0x00:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_PURPLE;
			break;
		case 0x04:
			local_color = COLOR_INDIGO;
			break;
		case 0x02:
			local_color = COLOR_YELLOW;
			break;
		case 0x01:
			local_color = COLOR_BLUE;
			break;
		case 0x0A:
			local_color = COLOR_RED;
			break;
		case 0x09:
			local_color = COLOR_GREEN;
			break;

		default:
		case 0x05:
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 2)
	{
		switch (combinaison)
		{

		case 0x00:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_RED;
			break;
		case 0x04:
			local_color = COLOR_YELLOW;
			break;
		case 0x02:
			local_color = COLOR_GREEN;
			break;
		case 0x01:
			local_color = COLOR_BLUE;
			break;
		case 0x03:
			local_color = COLOR_PURPLE;
			break;
		case 0x06:
			local_color = COLOR_INDIGO;
			break;

		default:
		case 0x0A:
		case 0x05:
		case 0x0C:
		case 0x09:
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 3)
	{
		switch (combinaison)
		{
		case 0x00:
			local_color = NO_COLOR;
			break;
		case 0x08:
			local_color = COLOR_RED;
			break;
		case 0x04:
			local_color = COLOR_BLUE;
			break;
		case 0x02:
			local_color = COLOR_GREEN;
			break;
		case 0x01:
			local_color = COLOR_YELLOW;
			break;
		case 0x06:
			local_color = COLOR_INDIGO;
			break;
		case 0x0A:
			local_color = COLOR_PURPLE;
			break;

		default:
		case 0x05:
		case 0x0C:
		case 0x03:
		case 0x09:
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 4)
	{
		switch (combinaison)
		{
		case 0x00:
			local_color = NO_COLOR;
			break;
		case 0x08:
		case 0x04:
			local_color = COLOR_BLUE;
			break;
		case 0x02:
			local_color = COLOR_INDIGO;
			break;
		case 0x01:
			local_color = COLOR_PURPLE;
			break;
		case 0x05:
			local_color = COLOR_RED;
			break;
		case 0x0A:
			local_color = COLOR_YELLOW;
			break;
		case 0x09:
			local_color = COLOR_GREEN;
			break;

		default:
		case 0x0C:
		case 0x06:
		case 0x03:
		case 0x0E:
		case 0x07:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else if (game_forms_id == 5)
	{
		switch (combinaison)
		{
		case 0x00:
			local_color = NO_COLOR;
			break;

		case 0x0F:
			local_color = COLOR_BLUE;
			break;
		case 0x0C:
			local_color = COLOR_GREEN;
			break;
		case 0x09:
			local_color = COLOR_PURPLE;
			break;
		case 0x0A:
			local_color = COLOR_INDIGO;
			break;
		case 0x03:
			local_color = COLOR_RED;
			break;
		case 0x07:
			local_color = COLOR_YELLOW;
			break;

		default:
		case 0x08:
		case 0x04:
		case 0x02:
		case 0x01:
		case 0x05:
		case 0x06:
		case 0x0E:
		case 0x0B:
		case 0x0D:
			local_color = COLOR_WHITE;
			break;
		}
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("Unknown game_forms_id!");
#endif
	}
	return local_color;
}

color_t getColorFromPixelArt(uint8_t combinaison, uint8_t game_forms_id)
{
	color_t local_color = NO_COLOR;

	if (game_forms_id == 1)
	{
		switch (combinaison)
		{
		case 0x00:
		default:
			local_color = NO_COLOR;
			break;

		case 0x08:	// NORD
			local_color = COLOR_BLUE;
			break;
		case 0x04:	// EST
			local_color = COLOR_WHITE;
			break;
		case 0x02:	// SUD
			local_color = COLOR_RED;
			break;
		case 0x01:	// OUEST
			local_color = COLOR_GREEN;
			break;
		}
	}
	else if (game_forms_id == 2)
	{
		switch (combinaison)
		{
		case 0x00:
		default:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_PURPLE;
			break;
		case 0x04:
			local_color = COLOR_YELLOW;
			break;
		case 0x02:
			local_color = COLOR_INDIGO;
			break;
		case 0x01:
			local_color = COLOR_PINK;
			break;
		}
	}
	else if (game_forms_id == 3)
	{
		switch (combinaison)
		{
		case 0x00:
		default:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_ORANGE;
			break;
		case 0x04:
			local_color = COLOR_WHITE;
			break;
		case 0x02:
			local_color = COLOR_INDIGO;
			break;
		case 0x01:
			local_color = COLOR_GREEN;
			break;
		}
	}
	else if (game_forms_id == 4)
	{
		switch (combinaison)
		{
		case 0x00:
		default:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_BLUE;
			break;
		case 0x04:
			local_color = COLOR_YELLOW;
			break;
		case 0x02:
			local_color = COLOR_RED;
			break;
		case 0x01:
			local_color = COLOR_PINK;
			break;
		}
	}
	else if (game_forms_id == 5)
	{
		switch (combinaison)
		{
		case 0x00:
		default:
			local_color = NO_COLOR;
			break;

		case 0x08:
			local_color = COLOR_PURPLE;
			break;
		case 0x04:
			local_color = COLOR_YELLOW;
			break;
		case 0x02:
			local_color = COLOR_INDIGO;
			break;
		case 0x01:
			local_color = COLOR_ORANGE;
			break;
		}
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("Unknown game_forms_id!");
#endif
	}
	return local_color;
}

#endif //__GAME_FORMS_H__
