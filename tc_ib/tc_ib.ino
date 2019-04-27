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

// Debugging Purpose
//#define DEBUG_SERIAL

#undef USE_SENSOR_LIGHT
#undef GET_SENSOR_LIGHT_WHEN_BLINK

//#define CONFIG_ALL_MODES
#define CONFIG_3_MODES

#define SLEEP_MODE
#define SLEEP_MODE_TEST

#include <Arduino.h>

// include libraries
// Button mgt
#include "Button.h"

// Color
#include "Color.h"

// Color config
#define INITIAL_COLOR				(color_t) COLOR_YELLOW
#define DEFAULT_IDLE_COLOR			(color_t) COLOR_GREEN
#define DEFAULT_SELECT_COLOR		(color_t) COLOR_RED
#define MODE_ID_COLOR				(color_t) COLOR_WHITE
#define MODE_ID_COLOR_V2			(color_t) COLOR_PURPLE
#define GAME_FORMS_ID_COLOR			(color_t) COLOR_YELLOW

// Detection voisins
boolean isChangeDetected = false;
#include "Neighbors.h"

// Light Sensor
#include "Intensity.h"

// LedRing
#include <Adafruit_NeoPixel.h>
#include "LedRing.h"

// Variables Etat
color_t colorId = INITIAL_COLOR;
uint8_t red, green, blue = 0;

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

// Options game_forms
#include "GameForms.h"
#include "GameFormsUtils.h"
#include "GameFormsFade.h"
uint8_t game_forms_id = 1;

/* Mode mgt  */
mode_t mode = MODE_1_GAME; // Mode sleep par defaut
mode_t modeNVM = MODE_1_GAME;

boolean modeConfig = false;
boolean mode_idle_blink = false;

boolean showingRules = false;
uint32_t previousMillisRule = 0;
#define SHOWRULE_TIMEOUT_MS			2000

color_t lastColorId = NO_COLOR;

/* Mode Custom Config*/
uint8_t modeCustomStep = 0;
#define MODE_CUSTOM_STEP_COMBI	0
#define MODE_CUSTOM_STEP_COLOR	1
boolean fakeClick = false;

/* Select Color v2*/
uint8_t select_color_combi = 0;
color_t select_color_id = COLOR_RED;

#include "GameLife.h"

/* NVM */
#include "NVM.h"

/* Sleep Mode Management */
#include "SleepMode.h"

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
	pinMode(LED_TOUCH_PANEL_PIN, INPUT_PULLUP);
	pinMode(LED_BUTTON_MODE_PIN, INPUT_PULLUP);

	// Init button state
	initButtonState(LED_TOUCH_PANEL_PIN);
	initButtonState(LED_BUTTON_MODE_PIN);

	/* Init neigbors detection */
	initDetectNeighbors();

	/* Check NVM */
	checkNVMConfigData();

	/* Read NVM */
	uint8_t unused_1;
	uint8_t unused_2;
	readNVMConfig((uint8_t *) &modeNVM, &game_forms_id, &unused_1, (uint8_t *) &unused_2);
	readNVMCombinaisonCustomConfig();

#ifdef DEBUG_SERIAL
	Serial.println(F("setup: End"));
#endif

#if 0
	boot_show_config(true);
#endif

	setup_init();
}

void loop()
{
	/* Check button events */
	button_event_t buttonEventTouchPanel = checkButton(LED_TOUCH_PANEL_PIN);
	button_event_t buttonEventMode = checkButton(LED_BUTTON_MODE_PIN);

#ifdef DEBUG_SERIAL
	if (buttonEventTouchPanel != NO_EVENT)
	{
		Serial.println("buttonEventTouchPanel " + String(buttonEventTouchPanel) + " " + String(millis()));
	}
	if (buttonEventMode != NO_EVENT)
	{
		Serial.println("buttonEventMode " + String(buttonEventMode) + " " + String(millis()));
	}
#endif
#ifdef SLEEP_MODE
	if (is_sleeping)
	{
		/* Override buttonEventTouchPanel detection */
		buttonEventTouchPanel = checkButtonStatus(LED_TOUCH_PANEL_PIN);

#ifdef SLEEP_MODE_TEST
		doLedRingBlink(COLOR_ORANGE, 200, 2);
#endif
	}
	else
#endif
	{
		if (!modeConfig)
		{
			process_mode_game(mode, buttonEventTouchPanel, buttonEventMode);
		}
		else
		{
			process_mode_config(mode, buttonEventTouchPanel, buttonEventMode);
		}

		// Check if switching mode is needed
		switch_mode(buttonEventTouchPanel, buttonEventMode);
	}

#ifdef SLEEP_MODE
	// Check if sleep is required
	if(go_to_sleep(buttonEventTouchPanel))
	{
		/* Just wake up, reset a few things */
		reset_init();

		/* Reset LED_TOUCH_PANEL_PIN to NOT go too quickly into sleep again */
		initButtonState(LED_TOUCH_PANEL_PIN);
	}
#endif
}

/*****************************************************************************
 * GESTION DES MODES
 */
#ifdef CONFIG_ALL_MODES
void process_mode_game(mode_t mode, button_event_t buttonEventTouchPanel, button_event_t buttonEventMode)
{
	switch (mode)
	{
		default:
		//mode = MODE_SLEEP;
		// No break
		case MODE_SLEEP:
		idle_sleep(buttonEventTouchPanel, buttonEventMode, colorId);
		break;

		case MODE_1_GAME:
		select_color(buttonEventTouchPanel, buttonEventMode);
		break;

		case MODE_1_GAME_V2:
		select_leds_color(buttonEventTouchPanel, buttonEventMode);
		break;

		case MODE_2A_GAME:
		game_forms_easy_all_rules(buttonEventTouchPanel);
		break;

		case MODE_2A_GAME_V2:
		game_forms_easy_combi_only(buttonEventTouchPanel);
		break;

		case MODE_2B_GAME:
		game_forms_easy_fade(buttonEventTouchPanel, buttonEventMode);
		break;

		case MODE_2B_GAME_V2:
		game_forms_easy_last_rule(buttonEventTouchPanel);
		break;

		case MODE_3_GAME:
		game_forms_hard(buttonEventTouchPanel);
		break;

		case MODE_3_GAME_V2:
		game_life(buttonEventTouchPanel);
		break;

		case MODE_4_CUSTOM:
		case MODE_4_CUSTOM_V2:
		game_forms_custom(buttonEventTouchPanel);
		break;
	}
}

void process_mode_config(mode_t mode, button_event_t buttonEventTouchPanel, button_event_t buttonEventMode)
{
	switch (mode)
	{
		default:
		case MODE_1_GAME:
		case MODE_1_GAME_V2:
		// TODO
		break;

		case MODE_2A_GAME:
		case MODE_2B_GAME:
		case MODE_3_GAME:
		case MODE_2A_GAME_V2:
		case MODE_2B_GAME_V2:
		mode_config(buttonEventTouchPanel);
		break;

		case MODE_3_GAME_V2:
		break;

		case MODE_4_CUSTOM:
		case MODE_4_CUSTOM_V2:
		mode_config_custom(buttonEventTouchPanel, buttonEventMode);
		break;
	}
}

void blink_when_switch_mode(uint8_t mode)
{
	// Specific blink
	switch ((mode_t) mode)
	{
		default:
		mode = MODE_SLEEP;
		doLedRingBlink(COLOR_PURPLE, 200, 3);
		break;

		case MODE_1_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_2A_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, NO_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_2B_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, NO_COLOR, MODE_ID_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_3_GAME:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, MODE_ID_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_4_CUSTOM:
		doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, MODE_ID_COLOR, MODE_ID_COLOR, 200, 3);
		break;

		case MODE_1_GAME_V2:
		doLedRingFourPixelsBlink(MODE_ID_COLOR_V2, NO_COLOR, NO_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_2A_GAME_V2:
		doLedRingFourPixelsBlink(MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, NO_COLOR, NO_COLOR, 200, 3);
		break;

		case MODE_2B_GAME_V2:
		doLedRingFourPixelsBlink(MODE_ID_COLOR_V2, NO_COLOR, MODE_ID_COLOR_V2, NO_COLOR, 200, 3);
		break;

		case MODE_3_GAME_V2:
		doLedRingFourPixelsBlink(MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, NO_COLOR, 200, 3);
		break;

		case MODE_4_CUSTOM_V2:
		doLedRingFourPixelsBlink(MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, MODE_ID_COLOR_V2, 200, 3);
		break;
	}
}

void switch_mode(button_event_t bTouchPanel, button_event_t bMode)
{
	mode_t local_mode = mode;
	boolean local_modeConfig = modeConfig;

	if (local_mode == MODE_SLEEP)
	{
		if (bTouchPanel == DOUBLE_CLICK)
		{
			mode = modeNVM;
		}
	}
	else if (modeConfig)
	{
		if (bMode == CLICK)
		{
			switch (mode)
			{
				case MODE_SLEEP:
				case MODE_1_GAME:
				case MODE_1_GAME_V2:
				default:
				break;

				case MODE_2A_GAME:
				case MODE_2A_GAME_V2:
				case MODE_2B_GAME:
				case MODE_2B_GAME_V2:
				case MODE_3_GAME:
				/* Exit config, Save game_forms_id */
				writeNVMConfig(NVM_GAME_FORMS_ID_OFF, game_forms_id);
				break;

				case MODE_3_GAME_V2:
				break;

				case MODE_4_CUSTOM:
				case MODE_4_CUSTOM_V2:
				/* Exit config, Save Custom Combinaison */
				writeNVMCombinaisonCustomConfig();
				break;
			}

			modeConfig = false;
		}
	}
	else
	{
		if (bMode == CLICK)
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
		}
		else if (bMode == HOLD)
		{
			modeConfig = true;
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
		/* Update RGB Status */
		colorToRGB(colorId, &red, &green, &blue, VALUE_COLOR_MAX);
		/* Notify change */
		isChangeDetected = true;
	}

	if (local_modeConfig != modeConfig)
	{
		switch (mode)
		{
			case MODE_SLEEP:
			case MODE_1_GAME:
			case MODE_1_GAME_V2:
			default:
			break;

			case MODE_2A_GAME:
			case MODE_2A_GAME_V2:
			case MODE_2B_GAME:
			case MODE_2B_GAME_V2:
			case MODE_3_GAME:
			/* Going into modeConfig for GAME 2A/2B/3 */
			showGameFormsId(game_forms_id, 200, 3, modeConfig);
			break;

			case MODE_3_GAME_V2:
			break;

			case MODE_4_CUSTOM:
			case MODE_4_CUSTOM_V2:
			/* Going into modeConfig for GAME 4 */
			doPixelRun(COLOR_WHITE, 300, 2);

			if (modeConfig)
			{
				modeCustomStep = MODE_CUSTOM_STEP_COMBI;
				fakeClick = true;
			}
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
		if (colorId == NO_COLOR)
		{
			colorId = DEFAULT_SELECT_COLOR;
		}
		break;

		case MODE_SLEEP:
		if (colorId == NO_COLOR)
		{
			colorId = DEFAULT_IDLE_COLOR;
		}
		break;

		case MODE_1_GAME_V2:
		colorId = NO_COLOR;
		fakeClick = true;
		break;

		case MODE_3_GAME_V2:
		colorId = NO_COLOR;
		life_init();
		break;

		case MODE_2B_GAME:
		fakeClick = true;
		break;

		default:
		colorId = NO_COLOR;
		break;
	}
}
#elif defined(CONFIG_3_MODES)
void process_mode_game(mode_t mode, button_event_t buttonEventTouchPanel, button_event_t buttonEventMode)
{
	(void) buttonEventMode;

	switch (mode)
	{
		default:
		case MODE_1_GAME:
			game_forms_easy_with_idle_sleep(buttonEventTouchPanel);
			break;

		case MODE_2A_GAME:
			game_forms_hard_with_idle_sleep(buttonEventTouchPanel);
			break;

		case MODE_2B_GAME:
			game_forms_easy_forced_sequence_with_idle_sleep(buttonEventTouchPanel);
			break;
	}
}

void process_mode_config(mode_t mode, button_event_t buttonEventTouchPanel, button_event_t buttonEventMode)
{
	(void) buttonEventMode;

	switch (mode)
	{
		default:
		case MODE_1_GAME:
		case MODE_2A_GAME:
		case MODE_2B_GAME:
			mode_config(buttonEventTouchPanel);
			break;
	}
}

void blink_when_switch_mode(uint8_t mode)
{
	// Specific blink
	switch ((mode_t) mode)
	{
		default:
		case MODE_1_GAME:
			doLedRingFourPixelsBlink(MODE_ID_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, 200, 3);
			break;

		case MODE_2A_GAME:
			doLedRingFourPixelsBlink(MODE_ID_COLOR, MODE_ID_COLOR, NO_COLOR, NO_COLOR, 200, 3);
			break;

		case MODE_2B_GAME:
			doLedRingFourPixelsBlink(MODE_ID_COLOR, NO_COLOR, MODE_ID_COLOR, NO_COLOR, 200, 3);
			break;
	}
}

void switch_mode(button_event_t bTouchPanel, button_event_t bMode)
{
	mode_t local_mode = mode;
	boolean local_modeConfig = modeConfig;

	if (modeConfig)
	{
		if (bMode == CLICK)
		{
			switch (mode)
			{
				default:
				case MODE_1_GAME:
				case MODE_2A_GAME:
				case MODE_2B_GAME:
					/* Exit config, Save game_forms_id */
					writeNVMConfig(NVM_GAME_FORMS_ID_OFF, game_forms_id);
					break;
			}

			modeConfig = false;
		}
	}
	else
	{
		if (bMode == CLICK)
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
		}
		else if (bMode == HOLD)
		{
			modeConfig = true;
		}

		/* Show */
		if (bTouchPanel == DOUBLE_CLICK)
		{
			showGameFormsId(game_forms_id, 200, 3, 0);
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
		/* Update RGB Status */
		colorToRGB(colorId, &red, &green, &blue, VALUE_COLOR_MAX);
		/* Notify change */
		isChangeDetected = true;
	}

	if (local_modeConfig != modeConfig)
	{
		switch (mode)
		{
			default:
			case MODE_1_GAME:
			case MODE_2A_GAME:
			case MODE_2B_GAME:
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
		case MODE_2A_GAME:
			colorId = NO_COLOR;
			break;

		case MODE_2B_GAME:
			colorId = NO_COLOR;
			initAllowedCombinaison();
			break;

		default:
			colorId = NO_COLOR;
			break;
	}
}
#endif
/*****************************************************************************
 * MODES
 */

// MODE SLEEP
void idle_sleep(button_event_t bTouchPanel, button_event_t bMode, color_t color_idle)
{
	(void) bTouchPanel;
	(void) bMode;

#if 0
	if(bTouchPanel == CLICK)
	{
		fade_speed++;
	}

	if(bMode == CLICK)
	{
		fade_speed--;
	}
#endif

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

#ifdef DEBUG_SERIAL
		//Serial.println("brightness " + String(brightness));
#endif
		if (brightness == 0)
		{
#ifdef USE_SENSOR_LIGHT
			// 0 to 80 by 2 with 30 ms => 40*30 = 1200 ms
			max_brightness = getIntensityWithLightSensorValue(readLightSensor());
			// fade_speed > 40/38 is NOK
			// fade_speed starting 10 is OK too
#endif

#ifdef DEBUG_SERIAL
			Serial.println("max_brightness " + String(max_brightness));
#endif
		}

		setLedringColor(color_idle, brightness);
	}
}

// MODE SLEEP BLINK
#define ILDE_SLEEP_BLINK_MS		5000
void idle_sleep_blink(button_event_t bTouchPanel, button_event_t bMode, color_t color_idle)
{
	(void) bTouchPanel;
	(void) bMode;

	if (millis() - previousMillisFade >= ILDE_SLEEP_BLINK_MS)
	{
		previousMillisFade = millis();
		doLedRingBlink(color_idle, 200, 2);
	}
}

void idle_sleep_dual_mode(boolean blink)
{
	if (blink)
	{
		idle_sleep_blink(NO_EVENT, NO_EVENT, COLOR_WHITE);
	}
	else
	{
		idle_sleep(NO_EVENT, NO_EVENT, lastColorId);
	}
}

void idle_sleep_switch_mode(uint8_t nb, boolean longIdleReached, boolean* blink)
{
	/* If in the game, idle is only blink */
	if (nb)
	{
		*blink = true;
	}
	/* If long timeout is reached, we switch back to idle sleep */
	if (longIdleReached)
	{
		*blink = false;
	}
}

// Game Forms with Fade
uint8_t rgb_idx;
uint8_t rgb[3];
uint8_t rgb_next[3];
uint8_t game_fade_speed = LWM_SPEED_MS;
// MODE JEU DES FORMES - EASY WITh FADE
void game_forms_easy_fade(button_event_t bTouchPanel, button_event_t bMode)
{
	(void) bMode;

	uint8_t combinaison;

	/* Refresh neighbors */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	if (bTouchPanel == CLICK || fakeClick == true)
	{
		fakeClick = false;

		/* Restart */
		rgb_idx = 0;
		colorId = getColorSequenceFromCombinaison(combinaison, game_forms_id, rgb_idx, &game_fade_speed);
		colorToRGB(colorId, &rgb[0], &rgb[1], &rgb[2], INTENSITY_MEDIUM_DEFAULT);
		setLedringRGB(rgb[0], rgb[1], rgb[2]);

		rgb_idx++;
		colorToRGB(colorId, &rgb_next[0], &rgb_next[1], &rgb_next[2], INTENSITY_MEDIUM_DEFAULT);
	}

	if ((millis() - previousMillisFade >= game_fade_speed) || isChangeDetected)
	{
		previousMillisFade = millis();
		boolean finished = true;

		for (uint8_t idx = 0; idx < 3; idx++)
		{
			if (rgb[idx] < rgb_next[idx])
			{
				rgb[idx] = rgb[idx] + 1;
				finished = false;
			}
			else if (rgb[idx] > rgb_next[idx])
			{
				rgb[idx] = rgb[idx] - 1;
				finished = false;
			}
		}

		if (finished || isChangeDetected)
		{
			/* Select next color */
			colorId = getColorSequenceFromCombinaison(combinaison, game_forms_id, rgb_idx, &game_fade_speed);
			rgb_idx++;
			if (rgb_idx > MAX_COLOR_SQ_NB - 1)
			{
				rgb_idx = 0;
			}
			colorToRGB(colorId, &rgb_next[0], &rgb_next[1], &rgb_next[2], INTENSITY_MEDIUM_DEFAULT);
		}

		setLedringRGB(rgb[0], rgb[1], rgb[2]);
	}
}

// MODE SELECTION COULEUR NORMAL
void select_color(button_event_t b, button_event_t bMode)
{
	(void) bMode;
	color_t previous_color = colorId;

	if (b == CLICK)
	{
		colorId = (color_t) (colorId + 1);
		if (colorId == NB_OF_COLOR)
		{
			colorId = (color_t) (NO_COLOR + 1);
		}
#ifdef DEBUG_SERIAL
		Serial.println("colorId " + String(colorId));
#endif
	}

	/* Update color if necessary*/
	if (previous_color != colorId)
	{
		/* Display new color*/
		setLedringColor(colorId, VALUE_COLOR_MAX);
		/* Update RGB Status */
		colorToRGB(colorId, &red, &green, &blue, VALUE_COLOR_MAX);
		/* Notify change */
		isChangeDetected = true;
	}
}
// MODE SELECTION COULEUR Leds by Leds
void select_leds_color(button_event_t b, button_event_t bMode)
{
	(void) bMode;

	if (b == CLICK || fakeClick == true)
	{
		fakeClick = false;

		/* Get next select_color_combi */
		select_color_combi++;
		if (select_color_combi > (NUMBER_OF_COMBINAISONS - 1))
		{
			/* Restart combi */
			select_color_combi = 1;

			/* Get next color */
			select_color_id = (color_t) (select_color_id + 1);
			if (select_color_id == NB_OF_COLOR)
			{
				select_color_id = (color_t) (NO_COLOR + 1);
			}
		}

#ifdef DEBUG_SERIAL
		Serial.println("select_color_combi: " + String(select_color_combi));
		Serial.println("select_color_id: " + String(select_color_id));
#endif

		/* Show it */
		showCombiWithColor(select_color_combi, select_color_id, 0, 0, 0);
		/* Notify change */
		isChangeDetected = true;
	}
}

// MODE JEU DES FORMES - EASY
void game_forms_easy(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbours */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Now we can play */
	colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

// MODE JEU DES FORMES - EASY - WITH IDLE SLEEP
void game_forms_easy_with_idle_sleep(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected);

	/* Check if idle is required */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected, &longIdle);
	/* Extra check for mode_idle_blink */
	idle_sleep_switch_mode(nb_of_neighbors, longIdle, &mode_idle_blink);

	if (goIdle)
	{
		idle_sleep_dual_mode(mode_idle_blink);
	}
	else
	{
		/* Warning: faces[i][HERE] must be 1 or 0 */
		combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

		/* Now we can play */
		colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId);
		games_forms_store_last_color(previous_color, colorId, &lastColorId);
	}
}

// MODE JEU DES FORMES - HARD - WITH IDLE SLEEP
void game_forms_hard_with_idle_sleep(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected);

	/* Check if idle is required */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected, &longIdle);
	/* Extra check for mode_idle_blink */
	idle_sleep_switch_mode(nb_of_neighbors, longIdle, &mode_idle_blink);

	if (goIdle)
	{
		idle_sleep_dual_mode(mode_idle_blink);
	}
	else
	{
		/* Warning: faces[i][HERE] must be 1 or 0 */
		combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

		/* Now we can play */
		colorId = getColorFromCombinaisonHard(combinaison, game_forms_id);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId);
		games_forms_store_last_color(previous_color, colorId, &lastColorId);
	}
}

// MODE JEU DES FORMES - EASY - WITH IDLE SLEEP
void game_forms_easy_forced_sequence_with_idle_sleep(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;
	uint8_t nb_of_neighbors = 0;
	boolean goIdle = false;
	boolean longIdle = false;

	/* Refresh neighbors */
	nb_of_neighbors = detectNeighbors(&isChangeDetected);

	/* Check if idle is required */
	goIdle = games_forms_idle_check(nb_of_neighbors, isChangeDetected, &longIdle);
	/* Extra check for mode_idle_blink */
	idle_sleep_switch_mode(nb_of_neighbors, longIdle, &mode_idle_blink);

	if (goIdle)
	{
		idle_sleep_dual_mode(mode_idle_blink);
	}
	else
	{
		/* Warning: faces[i][HERE] must be 1 or 0 */
		combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

		/* Now we can play */
		colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

		/* Filter colorID with allowed combinaison */
		colorId = getColorFromAllowedCombinaisonEasy(colorId);

		/* Update led ring */
		isChangeDetected = games_forms_update(previous_color, colorId);
		games_forms_store_last_color(previous_color, colorId, &lastColorId);
	}
}

// MODE JEU DES FORMES - EASY - WITH COLOR SEQUENCE FORCED
void game_forms_easy_forced_sequence(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbours */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Now we can play */
	colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

	/* Filter colorID with allowed combinaison */
	colorId = getColorFromAllowedCombinaisonEasy(colorId);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

// MODE JEU DES FORMES - EASY - SHOW ALL RULES
void game_forms_easy_all_rules(button_event_t bTouchPanel)
{
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbors */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Click and alone */
	if ((bTouchPanel == CLICK) && combinaison == 0x00)
	{
		// Show next rule and return
		showingRules = true;
#ifdef DEBUG_SERIAL
		/* Check if command executed OK */
		Serial.println("showing Next Rules!");
#endif
		// Show next rules
		showNextRules(game_forms_id);
		return;
	}
	/* if not alone anymore */
	if (combinaison != 0x00)
	{
		showingRules = false;
		resetShowNextRules();
	}
	if (showingRules)
	{
#ifdef DEBUG_SERIAL
		/* Check if command executed OK */
		Serial.println("showingRules!");
#endif
		return;
	}

	/* Now we can play */
	colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

// MODE JEU DES FORMES - EASY - SHOW COMBI
void game_forms_easy_combi_only(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbors */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Now we can play */
	colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

	/* Update led ring */
	isChangeDetected = games_forms_update_combi(previous_color, faces);

}

// MODE JEU DES FORMES - EASY - SHOW LAST RULE QUICKLY
void game_forms_easy_last_rule(button_event_t bTouchPanel)
{
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbors */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Click and alone */
	if ((bTouchPanel == CLICK) && combinaison == 0x00)
	{
		// Show next rule and return
		showingRules = true;
#ifdef DEBUG_SERIAL
		/* Check if command executed OK */
		Serial.println("Showing last rule");
#endif
		// We show lastColorId rule
		showRulesWithColor(game_forms_id, lastColorId);

		// Arm timer
		previousMillisRule = millis();
		return;
	}
	/* if not alone anymore */
	if (combinaison != 0x00)
	{
		showingRules = false;
	}
	if (showingRules)
	{
#ifdef DEBUG_SERIAL
		Serial.println("showingRules!");
#endif
		if (millis() - previousMillisRule >= SHOWRULE_TIMEOUT_MS)
		{
			showingRules = false;
			// We show rule 0 ( i.e: reset to nothing)
			showRules(game_forms_id, 0);
#ifdef DEBUG_SERIAL
			Serial.println("Stopped showingRules!");
#endif
		}
		else
		{
			return;
		}
	}

	/* Now we can play */
	colorId = getColorFromCombinaisonEasy(combinaison, game_forms_id);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

// MODE JEU DES FORMES - HARD
void game_forms_hard(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbours */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Now we can play */
	colorId = getColorFromCombinaisonHard(combinaison, game_forms_id);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

#ifdef DEBUG_SERIAL
uint32_t previousMillisChangeDetected = 0;
#endif

// ORGANISME VIVANT
void game_life(button_event_t bTouchPanel)
{
	/**/
	uint8_t temp_nb_neighbors;

	/* Switch to startingCell */
	life_processTouchPanel(bTouchPanel);

	/* Refresh neighbors */
	temp_nb_neighbors = detectNeighbors(&isChangeDetected);

#ifdef DEBUG_SERIAL
	if (isChangeDetected)
	{
		uint32_t diff = (uint32_t) (millis() - previousMillisChangeDetected);
		previousMillisChangeDetected = millis();
		Serial.println("temp_nb_neighbors = " + String(temp_nb_neighbors) + " " + String(diff));
	}
#endif
	/* Process depending on Cell job */
	if (startingCell)
	{
		life_processStartingCell();
	}
	else
	{
		life_processNormalCell(temp_nb_neighbors);
	}

	/* Timers for fading */
	life_fadingLeds();
}

// MODE JEU DES FORMES - CUSTOM
void game_forms_custom(button_event_t bTouchPanel)
{
	(void) bTouchPanel;
	uint8_t combinaison;
	color_t previous_color = colorId;

	/* Refresh neighbors */
	detectNeighbors(&isChangeDetected);

	/* Warning: faces[i][HERE] must be 1 or 0 */
	combinaison = 0x0F & ((faces[0] << 3) + (faces[1] << 2) + (faces[2] << 1) + faces[3]);

	/* Now we can play */
	colorId = getColorFromCombinaisonCustom(combinaison);

	/* Update led ring */
	isChangeDetected = games_forms_update(previous_color, colorId);
	games_forms_store_last_color(previous_color, colorId, &lastColorId);
}

void mode_config(button_event_t bTouchPanel)
{
	if (bTouchPanel == CLICK)
	{
		/* Increase game_forms_id */
		if (game_forms_id < 5)
		{
			game_forms_id++;
		}
		else
		{
			game_forms_id = 1;
		}

		/* Show game_forms_id */
		showGameFormsId(game_forms_id, 0, 0, 0);
	}
}

uint8_t local_combi_idx = 0;
color_t local_color_idx = NO_COLOR;

void mode_config_custom(button_event_t bTouchPanel, button_event_t bMode)
{
	(void) bMode;

	if (modeCustomStep == MODE_CUSTOM_STEP_COMBI)
	{
		/* show current combi with current color */
		if ((bTouchPanel == CLICK) || fakeClick)
		{
			fakeClick = false;
			/* Get next local_combi */
			local_combi_idx++;
			if (local_combi_idx > (NUMBER_OF_COMBINAISONS - 1))
			{
				local_combi_idx = 0;
			}

			//  Get current color for this combi and show
			local_color_idx = combinaisonCustom[local_combi_idx];
			if (local_color_idx == NO_COLOR)
			{
				local_color_idx = COLOR_WHITE;
			}

			/* Show it */
			showCombiWithColor(local_combi_idx, local_color_idx, 0, 0, 0);
#ifdef DEBUG_SERIAL
			/* Check if command executed OK */
			Serial.println("COMBI:Show combi = " + String(local_combi_idx) + " with color " + String(local_color_idx));
#endif
		}

		if (bTouchPanel == HOLD)
		{
			/* Blink !*/
			showCombiWithColor(local_combi_idx, local_color_idx, 200, 3, 1);

			modeCustomStep = MODE_CUSTOM_STEP_COLOR;
			fakeClick = true;

#ifdef DEBUG_SERIAL
			showCombiCustom();
#endif
		}
	}
	else if (modeCustomStep == MODE_CUSTOM_STEP_COLOR)
	{
		/* show current combi with current color */
		if ((bTouchPanel == CLICK) || fakeClick)
		{
			/* Get next local_color_idx */
			if (!fakeClick)
			{
				local_color_idx = (color_t) (local_color_idx + 1);
				if (local_color_idx > (NB_OF_COLOR - 1))
				{
					local_color_idx = COLOR_RED;
				}
			}

			fakeClick = false;

			/* Show it */
			showCombiWithColor(local_combi_idx, local_color_idx, 0, 0, 0);

#ifdef DEBUG_SERIAL
			/* Check if command executed OK */
			Serial.println("COLOR:Show combi = " + String(local_combi_idx) + " with color " + String(local_color_idx));
#endif
		}

		if (bTouchPanel == HOLD)
		{
			/* Blink !*/
			showCombiWithColor(local_combi_idx, local_color_idx, 200, 3, 1);

			/* Set custom combi */
			if (local_color_idx == COLOR_WHITE)
			{
				local_color_idx = NO_COLOR;
			}

			combinaisonCustom[local_combi_idx] = local_color_idx;
			modeCustomStep = MODE_CUSTOM_STEP_COMBI;
			fakeClick = true;

#ifdef DEBUG_SERIAL
			showCombiCustom();
#endif
		}
	}
}
/*********************************************************************************
 * DECLARATION FONCTIONS
 */
void boot_show_config(bool doPixel)
{
	// Pixel run with NVM_COLOR_DEFAULT_OFF
	if (doPixel)
	{
		doPixelRun(colorId, 400, 5);
	}

	//  Blink "game_forms_id" times with color_difficulty
	doLedRingBlink(COLOR_INDIGO, 200, game_forms_id);

}

void reset_init(void)
{
	/* Pick up a random color */
	colorId = (color_t) random(NO_COLOR + 1, NB_OF_COLOR);
	lastColorId = colorId;

	/* reset idle blink/sleep state*/
	mode_idle_blink = false;

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
