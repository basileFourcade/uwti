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

#define LONG_ALONE_TIMEOUT_S			(15*60)
#define VERY_LONG_ALONE_TIMEOUT_S  		(30*60)

uint32_t previousMillisAlone = 0;
uint32_t previousSecondsAlone = 0;

boolean games_forms_update(color_t previous_color, color_t current_color, boolean ischangeDetected)
{
	boolean changeDetected = false;

	/* Update color if necessary*/
	if ((previous_color != current_color) || ischangeDetected)
	{
#ifdef COLOR_CHANGES_FIXED_FREQUENCY
		/* Stack the new color */
		changesColorRequested = true;
		colorRequested = current_color;
#else
		/* Display new color*/
		setLedringColor(current_color, VALUE_COLOR_MEDIUM);
		/* Notify change */
		changeDetected = true;
#endif
	}

	return changeDetected;
}

void game_forms_reset_idle(void)
{
	/* Arm timer */
	previousSecondsAlone = 0;
	previousMillisAlone = millis();
}

boolean games_forms_idle_check(uint8_t nb_of_neighbors, boolean changeDetected,
		boolean* longTimeoutReached, boolean* veryLongTimeoutReached, color_t* randColor)
{
	boolean goIdle = false;

	if (nb_of_neighbors == 0 && changeDetected)
	{
		game_forms_reset_idle();

		/* Pick up a random color */
		*randColor = (color_t) random(NO_COLOR + 1, NB_OF_COLOR);
#ifdef DEBUG_SERIAL
		Serial.println(" Random colorId " + String(*randColor));
#endif
	}

	/* If still 0 neighbors */
	if (nb_of_neighbors == 0)
	{
		if ((millis() - previousMillisAlone >= 1000))
		{
			previousMillisAlone = millis();
			previousSecondsAlone++;
#ifdef DEBUG_SERIAL
			Serial.println(" previousSecondsAlone " + String(previousSecondsAlone));
#endif
		}

		if (previousSecondsAlone >= LONG_ALONE_TIMEOUT_S)
		{
			goIdle = true;
			*longTimeoutReached = true;
#ifdef DEBUG_SERIAL
			Serial.println("goIdle & longTimeoutReached");
#endif
#ifdef DEEP_SLEEP_WORK
			if (previousSecondsAlone >= VERY_LONG_ALONE_TIMEOUT_S)
			{
				*veryLongTimeoutReached = true;
			}
#endif
		}
	}

	return goIdle;
}
#endif //__GAME_FORMS_UTILS_H__
