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
#ifndef __NVM_H__
#define __NVM_H__

#include "EEPROM.h"

/* NVM mapping */
#define NVM_VERSION_OFF				0
#define NVM_VERSION_SIZE			4
#define NVM_VERSION_MAJOR			(uint8_t) 1
#define NVM_VERSION_MINOR			(uint8_t) 0
#define NVM_VERSION_OPT				(uint8_t) 0  /* Incremented each time NVM/BLE config needs to be changed */
#define NVM_VERSION_BUILD			(uint8_t) 3  /* Incremented at each release */

#define NVM_CONTROL_OFF				NVM_VERSION_OFF + NVM_VERSION_SIZE
#define NVM_CONTROL_SIZE			2
#define NVM_CONTROL_VALUE			(uint8_t) 0xBF

#define NVM_MODE_OFF				NVM_CONTROL_OFF + NVM_CONTROL_SIZE
#define NVM_MODE_SIZE				1
#define NVM_GAME_FORMS_ID_OFF		NVM_MODE_OFF + NVM_MODE_SIZE /* id */
#define NVM_GAME_FORMS_ID_SIZE		1
#define NVM_GAME_FORMS_LEVEL_OFF	NVM_GAME_FORMS_ID_OFF + NVM_GAME_FORMS_ID_SIZE /* level */
#define NVM_GAME_FORMS_LEVEL_SIZE	1
#define NVM_COLOR_DEFAULT_OFF		NVM_GAME_FORMS_LEVEL_OFF + NVM_GAME_FORMS_LEVEL_SIZE
#define NVM_COLOR_DEFAULT_SIZE		1

#define NVM_TOTAL_SIZE				NVM_COLOR_DEFAULT_OFF + NVM_COLOR_DEFAULT_SIZE

void resetNVMConfig(void)
{
#ifdef DEBUG_SERIAL
	Serial.println("resetNVMConfig");
#endif

	/* Version */
	EEPROM.write(NVM_VERSION_OFF, NVM_VERSION_MAJOR);
	EEPROM.write(NVM_VERSION_OFF + 1, NVM_VERSION_MINOR);
	EEPROM.write(NVM_VERSION_OFF + 2, NVM_VERSION_OPT);
	EEPROM.write(NVM_VERSION_OFF + 3, NVM_VERSION_BUILD);

	/* Control value */
	EEPROM.write(NVM_CONTROL_OFF, NVM_CONTROL_VALUE);
	EEPROM.write(NVM_CONTROL_OFF + 1, NVM_CONTROL_VALUE);

	/* Configuration */
	EEPROM.write(NVM_MODE_OFF, MODE_1_GAME); /* default mode: MODE_1_GAME */
	EEPROM.write(NVM_GAME_FORMS_ID_OFF, 1); /* default id: 1 */
	EEPROM.write(NVM_GAME_FORMS_LEVEL_OFF, 0); /* default level: DIFFICULTY_EASY */
	EEPROM.write(NVM_COLOR_DEFAULT_OFF, 1); /* default color: COLOR_RED */

}

void checkNVMConfigData(void)
{
	uint8_t value;
	bool resetNVMNeeded = false;

	/* First read control value */
	for (uint8_t i = NVM_CONTROL_OFF; i < NVM_CONTROL_OFF + NVM_CONTROL_SIZE;
			i++)
	{
		value = EEPROM.read(i);
		if (value != NVM_CONTROL_VALUE)
		{
			resetNVMNeeded = true;
			break;
		}
	}

	if (!resetNVMNeeded)
	{
		/* Then check version */
		for (uint8_t i = NVM_VERSION_OFF;
				i < NVM_VERSION_OFF + NVM_VERSION_SIZE; i++)
		{
			value = EEPROM.read(i);
			switch (i)
			{
			case NVM_VERSION_OFF:
				if (value < NVM_VERSION_MAJOR)
				{
					resetNVMNeeded = true;
				}
				break;
			case (NVM_VERSION_OFF + 1):
				if (value < NVM_VERSION_MINOR)
				{
					resetNVMNeeded = true;
				}
				break;
			case (NVM_VERSION_OFF + 2):
				if (value < NVM_VERSION_OPT)
				{
					resetNVMNeeded = true;
				}
				break;
			case (NVM_VERSION_OFF + 3):
				if (value < NVM_VERSION_BUILD)
				{
					resetNVMNeeded = true;
				}
				break;
			}

			if (resetNVMNeeded)
			{
				break;
			}
		}
	}
	if (resetNVMNeeded)
	{
#ifdef DEBUG_SERIAL
		Serial.println("resetNVMNeeded true");
#endif
		resetNVMConfig();
	}
}

void readNVMConfig(uint8_t* mode, uint8_t* game_id, uint8_t* game_lvl,
		uint8_t* default_color)
{
	*mode = EEPROM.read(NVM_MODE_OFF);
	*game_id = EEPROM.read(NVM_GAME_FORMS_ID_OFF);
	*game_lvl = EEPROM.read(NVM_GAME_FORMS_LEVEL_OFF);
	*default_color = EEPROM.read(NVM_COLOR_DEFAULT_OFF);

#ifdef DEBUG_SERIAL
	Serial.println("readNVMConfig :");
	Serial.println("mode :" + String(*mode));
	Serial.println("game_id :" + String(*game_id));
	Serial.println("game_lvl :" + String(*game_lvl));
	Serial.println("default_color :" + String(*default_color));
#endif
}

void writeNVMConfig(uint16_t address, uint8_t val)
{
	if (address < EEPROM.length())
	{
		EEPROM.write(address, val);
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("Out of bounds address");
#endif
	}
}

#endif //__NVM_H__
