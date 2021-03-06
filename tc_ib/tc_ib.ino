/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2019 Basile Fourcade & Arnaud Brachet
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

// Features/options
#undef FIXED_GAME_FORMS_ID
//#define GAME_FORMS_ID_VALUE		1	/* From 1 to 5 */
#undef COLOR_CHANGES_FIXED_FREQUENCY
#define DEEP_SLEEP_DONT_WORK

// Debugging Purpose
#undef DEBUG_SERIAL
#define SLEEP_MODE

#include <Arduino.h>

// include libraries
// Button mgt
#include "Button.h"

// Color
#include "Color.h"

// Color config
#define INITIAL_COLOR				(color_t) COLOR_YELLOW
#define MODE_ID_COLOR				(color_t) COLOR_WHITE
#define GAME_FORMS_ID_COLOR			(color_t) COLOR_YELLOW

// Detection voisins
boolean isChangeDetected = false;
boolean isFirstNeighborsDetected = false;
boolean isLastNeighborsLost = false;
boolean noNeighborsUntilNow = true;
#include "Neighbors.h"

// Light Sensor
#include "Intensity.h"

// LedRing
#include <Adafruit_NeoPixel.h>
#include "LedRing.h"

// Variables Etat
color_t colorId = INITIAL_COLOR;
color_t randomColorId = NO_COLOR;

/* MODE_SLEEP */
#define FADE_SPEED_MS   30
uint16_t fade_speed = FADE_SPEED_MS;
uint8_t fadeAmount = 2;
uint32_t previousMillisFade = 0;
uint8_t brightness = 0;
boolean fadeWayUp = true;
uint8_t max_brightness = VALUE_COLOR_MEDIUM;

// All mode definitions
#include "Mode.h"

#ifdef COLOR_CHANGES_FIXED_FREQUENCY
boolean changesColorRequested = true;
boolean resetFrequency = false;
color_t colorRequested = NO_COLOR;
#define FREQUENCY_COLOR_CHANGES_MS			(uint32_t) 1000
uint32_t previousMillisColorChanges = 0;
#endif

// Options game_forms
#include "GameForms.h"
#include "GameFormsUtils.h"
#include "GameFormsFade.h"
uint8_t game_forms_id = 1;

/* Mode mgt  */
mode_t mode = MODE_1_GAME; // Mode sleep par defaut
mode_t modeNVM = MODE_1_GAME;

boolean modeConfig = false;

/* NVM */
#include "NVM.h"

/* Sleep Mode Management */
#include "SleepMode.h"

/* Sleep Mode Management */
#include "Battery.h"


boolean sleepModeRequired = false;
/* the value is ised to choose between SLEEPING_TIME and DEEP_SLEEPING_TIME */
boolean deepSleep = false;
#ifdef DEEP_SLEEP_WORK
boolean deepSleepNotificationDone = false;
#endif
uint32_t millisSlept = 0;

/* Pixel Art Duo */
uint8_t firstNeighbor = 0;

/*****************************************************************************
 * UTILS
 */

void setup()
{

#ifdef DEBUG_SERIAL
	// So you have time to setup Serial Monitor
	delay(5000);

	// Init Serial
	Serial.begin(115200);
	Serial.println(F("setup: Start"));
#endif

	// Initialization LedRing
	strip.begin();
	strip.show();

	// Declaration INPUT / OUTPUT
	pinMode(LED_BUTTON_MODE_PIN, INPUT_PULLUP);
#if 0
	pinMode(RED_LED_PIN, OUTPUT);
	digitalWrite(RED_LED_PIN, 0);
#endif

	/* Charge Status PIN */
	pinMode(CHARGING_STATUS_PIN, INPUT_PULLUP);

	// Init button state
	initButtonState();

	/* Init neigbors detection */
	initDetectNeighbors();

	/* Check NVM */
	checkNVMConfigData();

	/* Read NVM */
	uint8_t unused_1;
	uint8_t unused_2;
	readNVMConfig((uint8_t *) &modeNVM, &game_forms_id, &unused_1,
			(uint8_t *) &unused_2);

#ifdef FIXED_GAME_FORMS_ID
	/* Overwrite game_forms_id*/
	game_forms_id = GAME_FORMS_ID_VALUE;
#endif

	setup_init();

	// Init Bat level
	batteryChargingStatusInit();

#ifdef DEBUG_SERIAL
	Serial.println(F("setup: End"));
#endif

}

void loop()
{
	/* Check button events */
	button_event_t buttonEventMode = checkButton(LED_BUTTON_MODE_PIN);

#ifdef DEBUG_SERIAL
	if (buttonEventMode != NO_EVENT)
	{
		Serial.println(
				"buttonEventMode " + String(buttonEventMode) + " "
				+ String(millis()));
	}
#endif
#ifdef SLEEP_MODE
	if (is_sleeping)
	{
		/* Check if neigbors or charging or button */
		if (detectNeighbors(&isChangeDetected, &isFirstNeighborsDetected,
				&isLastNeighborsLost, &noNeighborsUntilNow) || isCharging
				|| (checkButtonStatus(LED_BUTTON_MODE_PIN) != NO_EVENT))
		{
			/* If yes stop sleep mode */
			sleepModeRequired = false;
			/* NOTE FOR MYSELF: isChangeDetected may be consumed here
			 * Avoid using isChangeDetected for triggering an action
			 * */
		}
	}
	else
#endif
	{
		if (!modeConfig)
		{
			process_mode_game(mode, buttonEventMode);
		}
		else
		{
			process_mode_config(mode, buttonEventMode);
		}

		// Check if switching mode is needed
		switch_mode(buttonEventMode);
	}

#ifdef SLEEP_MODE
	// Check if sleep is required
	if (go_to_sleep(sleepModeRequired, &millisSlept, deepSleep))
	{
		/* Just wake up, reset a few things */
		reset_init();
		/* Reset Idle timers */
		game_forms_reset_idle();
#ifdef DEEP_SLEEP_WORK
		if(deepSleepNotificationDone)
		{
			deepSleepNotificationDone = false;
			doPixelRun(COLOR_BLUE, 1000, 1, PIXEL_RUN_ANTI_CLOCKWISE);
		}
#endif
	}
#endif

	/* Battery charge detection */
	if (batteryChargeDetection())
	{
		// Action on change detected
		if (!isCharging)
		{
			/* Charge is over, we reset the LED, games will overwrite if any */
			setLedringColor(NO_COLOR, 0);
		}
	}

	/* If we are charging and noNeighborsUntilNow is still true, then clear it */
	if(isCharging && noNeighborsUntilNow){
		noNeighborsUntilNow = false;
	}

	/* Battery level check - provide millisSlept */
	batteryLevelStatus(millisSlept);

	/* Notify with LedRun */
	batteryChargeNotify();

}

/*****************************************************************************
 * GESTION DES MODES
 */
void process_mode_game(mode_t mode, button_event_t buttonEventMode)
{
	(void) buttonEventMode;

	switch (mode)
	{
	default:
	case MODE_1_GAME:
		pixel_art_duo_with_idle_sleep(buttonEventMode);
		break;

	case MODE_2_GAME:
		game_forms_easy_with_idle_sleep(buttonEventMode);
		break;

	case MODE_3_GAME:
		game_forms_hard_with_idle_sleep(buttonEventMode);
		break;
	}

#ifdef COLOR_CHANGES_FIXED_FREQUENCY
	if ((millis() - previousMillisColorChanges >= FREQUENCY_COLOR_CHANGES_MS) || resetFrequency)
	{
		previousMillisColorChanges = millis();
		resetFrequency = false;

		if(changesColorRequested)
		{
			changesColorRequested = false;
			/* Display new color*/
			setLedringColor(colorRequested, VALUE_COLOR_MAX);
		}
	}
#endif
}

void process_mode_config(mode_t mode, button_event_t buttonEventMode)
{
	switch (mode)
	{
	default:
	case MODE_1_GAME:
	case MODE_2_GAME:
	case MODE_3_GAME:
		mode_config(buttonEventMode);
		break;
	}
}

#define BLINK_SWITCH_MODE_MS		500
void blink_when_switch_mode(uint8_t mode)
{
	// Specific blink
	switch ((mode_t) mode)
	{
	default:
	case MODE_1_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, NO_COLOR, NO_COLOR, NO_COLOR,
				BLINK_SWITCH_MODE_MS, 2);
		break;

	case MODE_2_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, NO_COLOR,
				NO_COLOR, BLINK_SWITCH_MODE_MS, 2);
		break;

	case MODE_3_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, MODE_ID_COLOR,
				NO_COLOR, BLINK_SWITCH_MODE_MS, 2);
		break;
	}
}

void switch_mode(button_event_t bMode)
{
	mode_t local_mode = mode;
	boolean local_modeConfig = modeConfig;

	if (modeConfig)
	{
		if (bMode == CLICK)
		{

		}
		else if (bMode == HOLD)
		{
			switch (mode)
			{
			default:
			case MODE_1_GAME:
			case MODE_2_GAME:
			case MODE_3_GAME:
				/* Exit config, Save game_forms_id */
				writeNVMConfig(NVM_GAME_FORMS_ID_OFF, game_forms_id);
				break;
			}

			modeConfig = false;
		}
	}
	else
	{
		if (bMode == DOUBLE_CLICK)
		{
			/* Cycle over all modes except SLEEP*/
			if (mode < (NB_OF_MODE - 1))
			{
				mode = (mode_t) (mode + 1);
			}
			else
			{
				mode = MODE_1_GAME;
			}
			/* Each time we switch mode, we store it */
			writeNVMConfig(NVM_MODE_OFF, mode);

#ifdef DEBUG_SERIAL
			Serial.println("mode is " + String(mode));
#endif
		}
		else if (bMode == HOLD)
		{
#ifdef DEBUG_SERIAL
			Serial.println("Go config is");
#endif
			modeConfig = true;
		} /* Show */
		else if (bMode == CLICK)
		{
			//showGameFormsId(game_forms_id, 200, 3, 0);
			/* Show battery level */
			showBatteryLevel();
		}
	}

	/* blink if mode has changed*/
	if (local_mode != mode)
	{
		blink_when_switch_mode(mode);
		/* Init mode (set a default color) */
		init_mode(mode);
		/* Display new color*/
		setLedringColor(colorId, VALUE_COLOR_MAX);
		/* Notify change */
		isChangeDetected = true;
	}

	if (local_modeConfig != modeConfig)
	{
		switch (mode)
		{
		default:
		case MODE_1_GAME:
		case MODE_2_GAME:
		case MODE_3_GAME:
			/* Going into modeConfig for all games */
			showGameFormsId(game_forms_id, 200, 3, modeConfig);
			break;
		}

		/* Notify change */
		isChangeDetected = true;
	}
}

void init_mode(uint8_t mode)
{
	switch ((mode_t) mode)
	{
	case MODE_1_GAME:
	case MODE_2_GAME:
	case MODE_3_GAME:
		colorId = NO_COLOR;
		break;
	default:
		colorId = NO_COLOR;
		break;
	}
}
/*****************************************************************************
 * MODES
 */

// MODE SLEEP
void idle_sleep(color_t color_idle)
{
	if (millis() - previousMillisFade >= fade_speed)
	{
		previousMillisFade = millis();

		/* Depending on fadeWayUp */
		if (fadeWayUp)
		{
			brightness = brightness + fadeAmount;
		}
		else
		{
			brightness = brightness - fadeAmount;
		}
		/* Switch way if necessary */
		if (brightness <= 0 || brightness >= max_brightness)
		{
			fadeWayUp = !fadeWayUp;
		}

		if (brightness == 0)
		{
			/* RFU */
		}

		setLedringColor(color_idle, brightness);
	}
}

#if 0
void idle_sleep_mode(boolean longIdle, boolean veryLongIdle,
		color_t randColorId, uint8_t isBatteryCharging)
{
	if ((longIdle && !veryLongIdle) || isBatteryCharging)
	{
		idle_sleep(randColorId);
	}
	else if (veryLongIdle)
	{
		// trigger sleep mode
		sleepModeRequired = true;
	}
}
#endif

// MODE JEU DES FORMES - EASY - WITH IDLE SLEEP
void game_forms_easy_with_idle_sleep(button_event_t buttonEventMode)
{
	(void) buttonEventMode;
	uint8_t combinaison;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;
	boolean veryLongIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected,
			&isFirstNeighborsDetected, &isLastNeighborsLost,
			&noNeighborsUntilNow);

#ifdef COLOR_CHANGES_FIXED_FREQUENCY
	if (isFirstNeighborsDetected || isLastNeighborsLost)
	{
		resetFrequency = true;
	}
#endif

	/* Check if idle is required */
	/* Activate Sleep mode longIdle without notifications */
	/* Activate Sleep mode veryLongIdle with a Led run */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected,
			&longIdle, &veryLongIdle, &randomColorId);

	if (noNeighborsUntilNow || (isCharging && (nb_of_neighbors == 0)))
	{ /* At boot we do idle_sleep as long as no neighbors */
		idle_sleep(randomColorId);
	}
	else if (goIdle)
	{
#ifdef DEEP_SLEEP_WORK
		if (veryLongIdle)
		{
			if (!deepSleepNotificationDone)
			{
				doPixelRun(COLOR_BLUE, 1000, 1, PIXEL_RUN_CLOCKWISE);
				deepSleepNotificationDone = true;
			}
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
#endif
		if (longIdle)
		{
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
		{
			/* Nothing */
		}
	}
	else
	{
		/* Warning: faces[i][HERE] must be 1 or 0 */
		combinaison = 0x0F
				& ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1)
						+ faces[3]);

		/* Now we can play */
		colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId,
				isChangeDetected);
	}
}

// MODE JEU DES FORMES - HARD - WITH IDLE SLEEP
void game_forms_hard_with_idle_sleep(button_event_t buttonEventMode)
{
	(void) buttonEventMode;
	uint8_t combinaison;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;
	boolean veryLongIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected,
			&isFirstNeighborsDetected, &isLastNeighborsLost,
			&noNeighborsUntilNow);

#ifdef COLOR_CHANGES_FIXED_FREQUENCY
	if (isFirstNeighborsDetected || isLastNeighborsLost)
	{
		resetFrequency = true;
	}
#endif

	/* Check if idle is required */
	/* Activate Sleep mode longIdle without notifications */
	/* Activate Sleep mode veryLongIdle with a Led run */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected,
			&longIdle, &veryLongIdle, &randomColorId);

	if (noNeighborsUntilNow || (isCharging && (nb_of_neighbors == 0)))
	{ /* At boot we do idle_sleep as long as no neighbors */
		idle_sleep(randomColorId);
	}
	else if (goIdle)
	{
#ifdef DEEP_SLEEP_WORK
		if (veryLongIdle)
		{
			if (!deepSleepNotificationDone)
			{
				doPixelRun(COLOR_BLUE, 1000, 1, PIXEL_RUN_CLOCKWISE);
				deepSleepNotificationDone = true;
			}
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
#endif
		if (longIdle)
		{
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
		{
			/* Nothing */
		}
	}
	else
	{
		/* Warning: faces[i][HERE] must be 1 or 0 */
		combinaison = 0x0F
				& ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1)
						+ faces[3]);

		/* Now we can play */
		colorId = getColorFromCombinaisonHard(combinaison, game_forms_id);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId,
				isChangeDetected);
	}
}

// MODE JEU DES FORMES - HARD - WITH IDLE SLEEP
void pixel_art_duo_with_idle_sleep(button_event_t buttonEventMode)
{
	(void) buttonEventMode;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;
	boolean veryLongIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected,
			&isFirstNeighborsDetected, &isLastNeighborsLost,
			&noNeighborsUntilNow);

#ifdef COLOR_CHANGES_FIXED_FREQUENCY
	if (isFirstNeighborsDetected || isLastNeighborsLost)
	{
		resetFrequency = true;
	}
#endif

	/* Check if idle is required */
	/* Activate Sleep mode longIdle without notifications */
	/* Activate Sleep mode veryLongIdle with a Led run */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected,
			&longIdle, &veryLongIdle, &randomColorId);

	if (noNeighborsUntilNow || (isCharging && (nb_of_neighbors == 0)))
	{ /* At boot we do idle_sleep as long as no neighbors */
		idle_sleep(randomColorId);
	}
	else if (goIdle)
	{
#ifdef DEEP_SLEEP_WORK
		if (veryLongIdle)
		{
			if (!deepSleepNotificationDone)
			{
				doPixelRun(COLOR_BLUE, 1000, 1, PIXEL_RUN_CLOCKWISE);
				deepSleepNotificationDone = true;
			}
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
#endif
		if (longIdle)
		{
			sleepModeRequired = true;
			deepSleep = true;
		}
		else
		{
			/* Nothing */
		}
	}
	else
	{
		/* Only the first Neighbors matters */
		/* If we come from 0 to 1 (or more) neighbors, then pick the color */
		if ((nb_of_neighbors >= 1))
		{
			if (firstNeighbor == 0)
			{
				/* Nominal case */
				if (nb_of_neighbors == 1)
				{
					firstNeighbor = 0x0F
							& ((faces[0] << 3) + (faces[1] << 2)
									+ (faces[2] << 1) + faces[3]);
				}
				else
				{
					/* In this case, just took the first bit available
					 * NOTE: code above is redundant with the one below */
					for (uint8_t i = 0; i < NB_FACES; i++)
					{
						if (faces[i])
						{
							firstNeighbor = 0x0F & ((faces[i] << (3 - i)));
							break;
						}
					}
				}
			}
		}
		else if (nb_of_neighbors == 0)
		{ /* If we come back to 0 , reset the color */
			firstNeighbor = 0;
		}

		/* Now we can play */
		colorId = getColorFromPixelArt(firstNeighbor, game_forms_id);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId,
				isChangeDetected);
	}
}

void mode_config(button_event_t bMode)
{
	if (bMode == CLICK)
	{
#ifdef FIXED_GAME_FORMS_ID
#else
		/* Increase game_forms_id */
		if (game_forms_id < 5)
		{
			game_forms_id++;
		}
		else
		{
			game_forms_id = 1;
		}
#endif
		/* Show game_forms_id */
		showGameFormsId(game_forms_id, 0, 0, 0);
	}
}

void reset_init(void)
{
	colorId = NO_COLOR;

	/* Pick up a random color */
	randomColorId = (color_t) random(NO_COLOR + 1, NB_OF_COLOR);

#ifdef DEBUG_SERIAL
	Serial.println(" Random colorId " + String(colorId));
#endif
}

void setup_init(void)
{
	/* Init seed */
	randomSeed(analogRead(PHOTO_RESIST));

	reset_init();
}

#if 0
void test_sequence(void)
{
	digitalWrite(RED_LED_PIN, 1);
	doPixelRun(COLOR_GREEN, 300, 3, PIXEL_RUN_CLOCKWISE);
	digitalWrite(RED_LED_PIN, 0);
}
#endif
