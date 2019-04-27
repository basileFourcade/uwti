// GAME_LIFE
#ifndef __GAME_LIFE_H__
#define __GAME_LIFE_H__

/* Option */
#define FADE_UP

enum life_state_t
{
	MODE_DEFAULT = 0,
	MODE_IN_NEEDS = 1,
	MODE_SATISFIED = 2,
	MODE_LOCK = 3,
	NB_OF_LIFE_MODE = 4,
};

boolean startingCell = false;
life_state_t life_state;
color_t conditions[NB_FACES]; /* From 0 to 3 */
color_t condition_pressure;

#define LOCK_TIMEOUT			1000*15
#define NEEDS_TIMEOUT			1000*5
#define BOUNCE_PROTECION		150

uint32_t previousMillisLock = 0;
uint32_t previousMillisNeeds = 0;
uint32_t previousMillisFadeLeds = 0;
uint32_t previousMillisBounce = 0;
uint8_t fade_intensity_down[NB_FACES];
uint8_t fade_intensity_up[NB_FACES];

#define FADE_LEDS_SPEED_MS   	30

#define NB_OF_PATHS				7
uint8_t paths[NB_OF_PATHS][4] = {
		{ 30, 0, 0, 1 },
		{ 60, 0, 1, 0 },
		{ 90, 1, 0, 0 },
		{ 100, 0, 1, 1 },
		{ 110, 1, 0, 1 },
		{ 120, 1, 1, 0 },
		{ 130, 1, 1, 1 }
};

uint8_t paths_idx;
// 4 Points	1/13	7.7%
// 3 Points	3/13	23,1%
// 2 Points	9/13	69,2%

void life_generatePath(uint8_t nb_voisins)
{
	(void) nb_voisins;
	uint8_t random_value = random(0, 130);
	paths_idx = NB_OF_PATHS - 1;

	/* TODO: take in account nb_voisins */

	for (uint8_t idx = 0; idx < NB_OF_PATHS; idx++)
	{
		if (random_value < paths[idx][0])
		{
			paths_idx = idx;
			break;
		}
	}
#ifdef DEBUG_SERIAL
	uint8_t nb = paths[paths_idx][1] + paths[paths_idx][2] + paths[paths_idx][3];
	Serial.println("nb: " + String(nb));
#endif
}

void life_fadingLeds_reset()
{
	memset(fade_intensity_down, 0x00, sizeof(fade_intensity_down));
	memset(fade_intensity_up, VALUE_COLOR_MEDIUM, sizeof(fade_intensity_up));
}

void life_init()
{
	life_state = MODE_DEFAULT;
	condition_pressure = COLOR_ORANGE;
	memset(conditions, 0x00, sizeof(conditions));
	life_fadingLeds_reset();
}

void life_processTouchPanel(button_event_t bTouchPanel)
{
	color_t startingColor;

	/* startingCell ? */
	if (bTouchPanel == HOLD)
	{
		/* Switch */
		startingCell = !startingCell;
		/* Reset*/
		life_init();

		/* Notify user */
		if (startingCell)
		{
#ifdef DEBUG_SERIAL
			Serial.println("Starting Cell YES ");
#endif
			startingColor = COLOR_RED;
		}
		else
		{
#ifdef DEBUG_SERIAL
			Serial.println("Starting Cell NO ");
#endif
			startingColor = COLOR_GREEN;
		}

		doLedRingBlink(startingColor, 200, 3);
	}
}

void life_processStartingCell(void)
{
	boolean conditions_needed;
	uint8_t temp;

	switch (life_state)
	{
		case MODE_DEFAULT:
			/* Generate one condition */
			temp = random(0, NB_FACES - 1);
			conditions[temp] = COLOR_RED;

			/* Light it */
			setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3], VALUE_COLOR_MEDIUM);

			/* Switch state */
			life_state = MODE_IN_NEEDS;
			break;

		case MODE_IN_NEEDS:
			/* By default we are satisfied unless ... */
			conditions_needed = false;

			for (uint8_t idx = 0; idx < NB_FACES; idx++)
			{
				/* Conditions needed ? */
				if (conditions[idx] != 0)
				{
					/* Neighbors here ? */
					if (faces[idx] == 1)
					{
						if (conditions[idx] == COLOR_RED)
						{
							/* switch condition */
							conditions[idx] = COLOR_GREEN;
							/* Light it */
							setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3],
									VALUE_COLOR_MEDIUM);
						}
					}
				}
				else
				{
					if (faces[idx] == 1)
					{
						// we can ignore
					}
				}

				/* Check all conditions */
				if (conditions[idx] == COLOR_RED)
				{
					conditions_needed = true;
				}
			}

			/* switch life_state ?*/
			if (!conditions_needed)
			{
				life_state = MODE_SATISFIED;
			}
			break;

		case MODE_SATISFIED:
#if 0
			for (uint8_t idx = 0; idx < NB_FACES; idx++)
			{
				/* Conditions satisfied ? */
				if (conditions[idx] != 0)
				{
					/* if neighbors not here , we switch life_state */
					if (faces[idx] != 1)
					{
						conditions[idx] = COLOR_RED;
						/* Light it */
						setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3],
								VALUE_COLOR_MEDIUM);

						/* switch life_state */
						life_state = MODE_IN_NEEDS;
					}
				}
			}
#else
			/* The cell is satisfied, we switch to normal */
			startingCell = false;
#endif
			break;

		case MODE_LOCK:
		default:
			// Should not happen
			break;
	}
}

boolean life_is_fadingLeds_finished(uint8_t up_down)
{
	uint16_t sum = 0;
	boolean over = true;

	/* down case*/
	if (!up_down)
	{
		for (uint8_t idx = 0; idx < NB_FACES; idx++)
		{
			sum += fade_intensity_down[idx];
		}
		if (sum)
		{
			over = false;
		}
	}

	return over;
}

void life_processNormalCell(uint8_t nb_voisins)
{
	boolean conditions_needed = false;
	uint8_t tmp_idx = 1;

	switch (life_state)
	{
		case MODE_DEFAULT:
			if (nb_voisins > 0)
			{
				/* Update conditions: */
				/* one COLOR_GREEN for the touching one */
				/* random for the others */
				life_generatePath(nb_voisins);

				for (uint8_t idx = 0; idx < NB_FACES; idx++)
				{
					if (faces[idx] == 1)
					{
						conditions[idx] = COLOR_GREEN;
					}
					else
					{
						if (paths[paths_idx][tmp_idx++])
						{
							conditions[idx] = condition_pressure;
						}
						else
						{
							conditions[idx] = NO_COLOR;
						}
					}
#ifdef DEBUG_SERIAL
					Serial.println("Conditions " + String(idx) + " " + String(conditions[idx]));
#endif
				}

				/* Light it */
				setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3], VALUE_COLOR_MEDIUM);

				/* Switch life_state */
				life_state = MODE_IN_NEEDS;
				/* Trigger "needs" timeout */
				previousMillisNeeds = millis();
				/* Trigger bounce protection */
				previousMillisBounce = millis();
#ifdef DEBUG_SERIAL
				Serial.println("New state " + String(life_state));
#endif
			}
			else
			{
				// Nothing
			}
			break;

		case MODE_IN_NEEDS:
			if (nb_voisins > 0)
			{
				/* Reset flag Conditions needed */
				conditions_needed = false;

				for (uint8_t idx = 0; idx < NB_FACES; idx++)
				{
					/* Conditions needed ? */
					if (conditions[idx] != 0)
					{
						/* Neighbors here ? */
						if (faces[idx] == 1)
						{
							/* Conditions satisfied */
							if ((conditions[idx] == COLOR_RED) || (conditions[idx] == COLOR_ORANGE))
							{
								/* switch condition */
								conditions[idx] = COLOR_GREEN;
								/* Light it */
								setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3],
										VALUE_COLOR_MEDIUM);

								/* Reset "needs" timeout */
								previousMillisNeeds = millis();
#ifdef DEBUG_SERIAL
								Serial.println("condition satisfied");
#endif
							}
						}
						else
						{
							/* Conditions needed */
							conditions_needed = true;

							if (conditions[idx] == COLOR_GREEN)
							{
								/* switch condition */
								conditions[idx] = condition_pressure;
								/* Light it */
								setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3],
										VALUE_COLOR_MEDIUM);
							}
						}
					}
					else
					{
						if (faces[idx] == 1)
						{
							/* Switch life_state to LOCK */
							life_state = MODE_LOCK;
#ifdef DEBUG_SERIAL
							Serial.println("MODE_LOCK:Wrong faces[idx]");
#endif
							break;
						}
					}
				}
			}
			else
			{
				if ((millis() - previousMillisBounce) > BOUNCE_PROTECION)
				{
					/* Switch life_state to LOCK */
					life_state = MODE_LOCK;
#ifdef DEBUG_SERIAL
					Serial.println("MODE_LOCK:temp_nb_neighbors=0");
#endif
				}
				else
				{
					/* Conditions needed */
					conditions_needed = true;
#ifdef DEBUG_SERIAL
					Serial.println("BOUNCE_PROTECION");
#endif
				}
			}

			/* switch life_state ? */
			if ((life_state != MODE_LOCK) && !conditions_needed)
			{
				life_state = MODE_SATISFIED;
#ifdef DEBUG_SERIAL
				Serial.println("MODE_SATISFIED: !conditions_needed");
#endif
			}

			/* Still not satisfied ? */
			if (millis() - previousMillisNeeds >= NEEDS_TIMEOUT)
			{
				if (condition_pressure == COLOR_ORANGE)
				{
#ifdef DEBUG_SERIAL
					Serial.println("condition_pressure is RED");
#endif
					/* switch to RED */
					condition_pressure = COLOR_RED;
					/* Update conditions */
					for (uint8_t idx = 0; idx < NB_FACES; idx++)
					{
						/* Conditions needed */
						if (conditions[idx] == COLOR_ORANGE)
						{
							conditions[idx] = COLOR_RED;
						}
					}
					/* Light it */
					setLedringFourPixels(conditions[0], conditions[1], conditions[2], conditions[3],
							VALUE_COLOR_MEDIUM);

					/* Reset "needs" timeout */
					previousMillisNeeds = millis();
				}
				else
				{
					/* Switch life_state to LOCK */
					life_state = MODE_LOCK;
#ifdef DEBUG_SERIAL
					Serial.println("MODE_LOCK:NEEDS_TIMEOUT");
#endif
				}
			}

			if (life_state == MODE_LOCK)
			{
				/* reset any fading LEDS */
				life_fadingLeds_reset();
				/* Notify it */
				doLedRingBlink(COLOR_RED, 200, 3);
				/* Trigger Lock Timeout */
				previousMillisLock = millis();
			}
			break;

		case MODE_SATISFIED:
			for (uint8_t idx = 0; idx < NB_FACES; idx++)
			{
				/* Conditions satisfied ? */
				if (conditions[idx] != 0)
				{
					/* if neighbors not here anymore */
					if (faces[idx] != 1)
					{
						/* We remove and fade it down */
						conditions[idx] = NO_COLOR;
						/* we fade it down */
						fade_intensity_down[idx] = fade_intensity_up[idx];
						fade_intensity_up[idx] = VALUE_COLOR_MEDIUM;
					}
				}
#ifdef FADE_UP
				else
				{
					if (faces[idx] != 0)
					{
						/* New neighbor */
						/* We add and fade it up */
						conditions[idx] = COLOR_GREEN;
						/* we fade it up */
						fade_intensity_up[idx] = fade_intensity_down[idx];
						fade_intensity_down[idx] = 0;
					}
				}
#endif
			}
			/* If no more neighbors, get to default */
			if (nb_voisins == 0)
			{
				if (life_is_fadingLeds_finished(0))
				{
					life_init();
#ifdef DEBUG_SERIAL
					Serial.println("MODE_DEFAULT:temp_nb_neighbors == 0");
#endif
				}
			}
			break;

		case MODE_LOCK:
			/* Timer for MODE_LOCK */
			if (millis() - previousMillisLock >= LOCK_TIMEOUT)
			{
				/* Blink green before going back to MODE_DEFAULT */
				life_init();
#ifdef DEBUG_SERIAL
				Serial.println("MODE_DEFAULT:LOCK_TIMEOUT");
#endif
				/* Notify it */
				doLedRingBlink(COLOR_GREEN, 200, 3);
			}
			else
			{
				doPixelRun(COLOR_RED, 200, 2);
			}
			break;

		default:
			break;
	}
}

void life_fadingLeds()
{
	if (millis() - previousMillisFadeLeds >= FADE_LEDS_SPEED_MS)
	{
		previousMillisFadeLeds = millis();
		for (uint8_t idx = 0; idx < NB_FACES; idx++)
		{
			if (fade_intensity_down[idx] > 0)
			{
				fade_intensity_down[idx]--;
				setLedringOnePixel(idx, COLOR_GREEN, fade_intensity_down[idx]);
			}
#ifdef FADE_UP
			if (fade_intensity_up[idx] < VALUE_COLOR_MEDIUM)
			{
				fade_intensity_up[idx]++;
				setLedringOnePixel(idx, COLOR_GREEN, fade_intensity_up[idx]);
			}
#endif
		}
	}
}
#endif //__GAME_LIFE_H__
