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

#undef SLEEP_NOTIFIY_TEST

#ifdef SLEEP_NOTIFIY_TEST
#define SLEEP_IN_ID_COLOR		(color_t) COLOR_ORANGE
#define SLEEP_OUT_ID_COLOR		(color_t) COLOR_ORANGE
#endif

boolean is_sleeping = false;

boolean go_to_sleep(boolean goToSleep, uint32_t* millisSlept)
{
	boolean justWakingUp = false;
	*millisSlept = 0;

	/* If sleeping */
	if (is_sleeping)
	{
		if (!goToSleep)
		{
			/* wake up */
			is_sleeping = false;

#ifdef SLEEP_NOTIFIY_TEST
			/* Notify */
			doLedRingBlink(SLEEP_OUT_ID_COLOR, 200, 3);
#endif
			justWakingUp = true;
		}
		else
		{
			/* Keep sleeping */
			*millisSlept = Watchdog.sleep(SLEEPING_TIME);
		}
	}
	else
	{ /* If NOT sleeping */
		if (goToSleep)
		{
			is_sleeping = true;
			/* Notify */
#ifdef SLEEP_NOTIFIY_TEST
			/* Implicit shut down LEDS */
			doLedRingBlink(SLEEP_IN_ID_COLOR, 200, 3);
#else
			/* Explicitely shut down LEDS */
			setLedringColor(NO_COLOR, 0);
#endif
			/* Now sleep */
			*millisSlept = Watchdog.sleep(SLEEPING_TIME);
		}
	}

	return justWakingUp;
}

#endif // __SLEEP_MODE_H__
