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
/*
 * Battery.h
 *
 *  Created on: 28 avr. 2019
 *      Author: Basile
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#define GAP_WHEN_CHARGE		3
#define BATTERY_READ_MS		1000

#define BATTERY_LEVEL_20	635 //500
#define BATTERY_LEVEL_10	631 //480

#define BLINK_SPEED_20		30000
#define BLINK_SPEED_10		5000
#define BLINK_DURATION		1000

const uint8_t BATTERY_LEVEL = A2;

uint32_t previousMillisBattery = 0;
uint8_t isCharging = 0;
uint32_t batLevelPrevious = 0;
uint32_t batLevelLast = 0;

uint32_t previousMillisBlink = 0;

uint32_t readBatteryLevel(void)
{
	uint32_t sensor_value = analogRead(BATTERY_LEVEL);

#ifdef DEBUG_SERIAL
	Serial.println("battery = " + String(sensor_value));
#endif

	return sensor_value;
}

void batteryChargingStatusInit(void)
{
	isCharging = 1;

	// Do first measure
	batLevelLast = readBatteryLevel();
	batLevelPrevious = batLevelLast;

	previousMillisBattery = BATTERY_READ_MS;
}

uint8_t batteryChargingStatus()
{
	uint8_t changeDetected = 0;
	int32_t delta = 0;

	if (millis() - previousMillisBattery >= BATTERY_READ_MS)
	{
		previousMillisBattery = millis();

		batLevelPrevious = batLevelLast;
		batLevelLast = readBatteryLevel();

		delta = batLevelLast - batLevelPrevious;

#ifdef DEBUG_SERIAL
		Serial.println("batLevelPrevious = " + String(batLevelPrevious) +
				" batLevelLast = " + String(batLevelLast) +
				" delta = " + String(delta));
#endif

		if (delta >= GAP_WHEN_CHARGE)
		{
			isCharging = 1;
			changeDetected = 1;
#ifdef DEBUG_SERIAL
			Serial.println("isCharging = 1");
#endif
		}
		else if (delta <= - GAP_WHEN_CHARGE)
		{
			isCharging = 0;
			changeDetected = 1;
#ifdef DEBUG_SERIAL
			Serial.println("isCharging = 0");
#endif
		}
	}

	return changeDetected;
}

void batteryChargeNotify(void)
{
	uint32_t blink_speed = 0;

	if (batLevelLast < BATTERY_LEVEL_10)
	{
		// Blink fast starting now
		if (!blink_speed)
		{
			previousMillisBlink = blink_speed;
		}
		blink_speed = BLINK_SPEED_10;
	}
	else if (batLevelLast < BATTERY_LEVEL_20)
	{
		// Blink normal starting now
		if (!blink_speed)
		{
			previousMillisBlink = blink_speed;
		}
		blink_speed = BLINK_SPEED_20;
	}
	else
	{
		// no blink
		blink_speed = 0;
	}

	if (blink_speed)
	{
		if ((millis() - previousMillisBlink) > blink_speed)
		{
			digitalWrite(RED_LED_PIN, 1);

			if ((millis() - previousMillisBlink)
					> (blink_speed + BLINK_DURATION))
			{
				digitalWrite(RED_LED_PIN, 0);
				previousMillisBlink = millis();
			}
		}
	}
}
#endif /* BATTERY_H_ */
