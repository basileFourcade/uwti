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
