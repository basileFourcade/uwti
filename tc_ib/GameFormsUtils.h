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
#ifndef __GAME_FORMS_UTILS_H__
#define __GAME_FORMS_UTILS_H__

uint32_t previousMillisLastColor = 0;
#define LAST_COLOR_TIMEOUT_MS		2000

uint32_t previousMillisAlone = 0;
#define ALONE_TIMEOUT_MS			5000
#define LONG_ALONE_TIMEOUT_MS		5*60000

boolean games_forms_update(color_t previous_color, color_t current_color)
{
	boolean changeDetected = false;

	/* Update color if necessary*/
	if (previous_color != current_color)
	{
		/* Display new color*/
		setLedringColor(current_color, VALUE_COLOR_MAX);
		/* Update RGB Status */
		colorToRGB(current_color, NULL, NULL, NULL, VALUE_COLOR_MAX);
		/* Notify change */
		changeDetected = true;
		/* Trigger timer */
		previousMillisLastColor = millis();
	}

	return changeDetected;
}

boolean games_forms_update_combi(color_t previous_color,
		uint8_t faces[NB_FACES])
{
	boolean changeDetected = false;

	/* Update color if necessary*/
	if (previous_color != colorId)
	{
		/* Display new color*/
		setLedringFourPixels((color_t)(faces[0] * colorId),
				(color_t)(faces[1] * colorId), (color_t)(faces[2] * colorId),
				(color_t)(faces[3] * colorId), VALUE_COLOR_MAX);

		/* Notify change */
		changeDetected = true;
	}

	return changeDetected;
}

void games_forms_store_last_color(color_t previous_color, color_t current_color,
		color_t* last_color)
{
	/* Update color if necessary*/
	if (previous_color == current_color)
	{
		if (millis() - previousMillisLastColor >= LAST_COLOR_TIMEOUT_MS)
		{
			/* If colorId has been there sufficient time, store it */
			/* And it is not NO_COLOR*/
			if (current_color != NO_COLOR)
			{
				*last_color = current_color;
#ifdef DEBUG_SERIAL
				Serial.println("lastColorId = " + String(* last_color));
#endif
			}
		}
	}
}

boolean games_forms_idle_check(uint8_t nb_of_neighbors, boolean changeDetected,
		boolean* longTimeoutReached)
{
	boolean goIdle = false;

	if (nb_of_neighbors == 0 && changeDetected)
	{
		// Arm timer
		previousMillisAlone = millis();
	}

	/* If still 0 neighbors */
	if (nb_of_neighbors == 0)
	{
		if (((millis() > ALONE_TIMEOUT_MS)
				&& (millis() - previousMillisAlone >= ALONE_TIMEOUT_MS))
				|| (millis() < ALONE_TIMEOUT_MS))
		{
			goIdle = true;

			if (millis() - previousMillisAlone >= LONG_ALONE_TIMEOUT_MS)
			{
				*longTimeoutReached = true;
			}

#ifdef DEBUG_SERIAL
			//Serial.println("goIdle is true ");
#endif
		}
	}

	return goIdle;
}
#endif //__GAME_FORMS_UTILS_H__
