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
#ifndef __SLEEP_MODE_H__
#define __SLEEP_MODE_H__

#include <Adafruit_SleepyDog.h>
#define SLEEPING_TIME			4000
#define SLEEP_IN_ID_COLOR		(color_t) COLOR_ORANGE
#define SLEEP_OUT_ID_COLOR		(color_t) COLOR_ORANGE

boolean is_sleeping = false;

boolean go_to_sleep(button_event_t bTouchPanel)
{
	boolean justWakingUp = false;

	/* If sleeping */
	if (is_sleeping)
	{
		if (bTouchPanel != NO_EVENT)
		{
			/* wake up */
			is_sleeping = false;

			/* Notify */
			doLedRingBlink(SLEEP_OUT_ID_COLOR, 200, 3);

			justWakingUp = true;
		}
		else
		{
			/* Keep sleeping */
			Watchdog.sleep(SLEEPING_TIME);
		}
	}
	else
	{ /* If NOT sleeping */
		if (bTouchPanel == HOLD)
		{
			is_sleeping = true;
			/* Notify */
			/* Implicit shut down LEDS */
			doLedRingBlink(SLEEP_IN_ID_COLOR, 200, 3);

			/* Now sleep */
			Watchdog.sleep(SLEEPING_TIME);
		}
	}

	return justWakingUp;
}

#endif // __SLEEP_MODE_H__
