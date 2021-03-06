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

#define BATTERY_LEVEL_90	636
#define BATTERY_LEVEL_80	616
#define BATTERY_LEVEL_60	601
#define BATTERY_LEVEL_40	588
#define BATTERY_LEVEL_20	575
#define BATTERY_LEVEL_15	567
#define BATTERY_LEVEL_10	559

#define BLINK_SPEED_20		30000
#define BLINK_SPEED_10		5000
#define BLINK_DURATION		1000

const uint8_t BATTERY_LEVEL = A2;
const uint8_t CHARGING_STATUS_PIN = 1;
#if 0
#define RED_LED_PIN			A4 // LED TEST
uint8_t redLedPinState = 0;
#endif

uint32_t previousMillisBattery = 0;
uint8_t isCharging = 0;

#define NB_OF_SAMPLES		4
uint8_t batLevelLast_idx = 0;
uint32_t batLevelLast[NB_OF_SAMPLES] =
{ 0 };

#define NB_OF_AVG_VALUES		5
uint32_t batLevelAvg[NB_OF_AVG_VALUES] =
{ 0 };

uint32_t previousMillisPixelRun = 0;

uint32_t readBatteryLevel(void)
{
	uint32_t sensor_value = analogRead(BATTERY_LEVEL);

#ifdef DEBUG_SERIAL
	//Serial.println("battery = " + String(sensor_value));
#endif

	return sensor_value;
}

void batteryChargingStatusInit(void)
{
	isCharging = 0;
#if 0
	redLedPinState = 0;
#endif

	// Do first measure
	batLevelLast_idx = 0;
	batLevelLast[batLevelLast_idx] = readBatteryLevel();

	for (uint8_t i = 1; i < NB_OF_SAMPLES; i++)
	{
		batLevelLast[i] = batLevelLast[batLevelLast_idx];
	}

	for (uint8_t i = 1; i < NB_OF_AVG_VALUES; i++)
	{
		batLevelAvg[i] = batLevelLast[batLevelLast_idx];
	}

	previousMillisBattery = BATTERY_READ_MS;
}

uint8_t batteryLevelStatus(uint32_t millis_while_sleeping)
{
	uint8_t changeDetected = 0;
	int32_t delta = 0;

	// Sleep does not increment millis
	if (previousMillisBattery > millis_while_sleeping)
	{
		if (millis_while_sleeping > BATTERY_READ_MS)
		{
			previousMillisBattery -= BATTERY_READ_MS;
		}
	}
	else
	{
		previousMillisBattery = 0;
	}

	if (millis() - previousMillisBattery >= BATTERY_READ_MS)
	{
		previousMillisBattery = millis();

		/* Get a new measure */
		batLevelLast[batLevelLast_idx++] = readBatteryLevel();

		batLevelLast_idx = batLevelLast_idx % NB_OF_SAMPLES;

		/* Shift the avg measure */
		for (uint8_t i = 0; i < (NB_OF_AVG_VALUES - 1); i++)
		{
			batLevelAvg[i] = batLevelAvg[i + 1];
#ifdef DEBUG_SERIAL
			//Serial.println("batLevelAvg " + String(i) + " " + String(batLevelAvg[i]));
#endif
		}
		/* compute a new one */
		batLevelAvg[NB_OF_AVG_VALUES - 1] = (batLevelLast[0] + batLevelLast[1]
				+ batLevelLast[2] + batLevelLast[3]) >> 2;

#ifdef TEST_IF_CHARGING
		/* then compare the latest with the oldest */
		delta = batLevelAvg[NB_OF_AVG_VALUES - 1] - batLevelAvg[0];
#else
		(void) delta;
#endif

#ifdef DEBUG_SERIAL
		Serial.println("batLevelAvg[0] = " + String(batLevelAvg[0]) +
				" batLevelAvg[4] = " + String(batLevelAvg[NB_OF_AVG_VALUES - 1]) +
				" delta = " + String(delta));
#endif

#ifdef TEST_IF_CHARGING
		if (delta >= GAP_WHEN_CHARGE)
		{
			isCharging = 1;
			changeDetected = 1;

#undef TEST_BAT_DELTA
#ifdef TEST_BAT_DELTA
			ledShow12bits(delta);
			delay(1000);
#endif

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

#ifdef TEST_BAT_DELTA
			ledShow12bits(-delta);
			delay(1000);
#endif

		}
#undef TEST_BAT
#ifdef TEST_BAT
		ledShow12bits(batLevelAvg[NB_OF_AVG_VALUES - 1]);
		delay(500);
		setLedringColor(NO_COLOR, 0);
#endif
#endif
	}

	return changeDetected;
}

#define FREQUENCY_NOTIFICATION_LOW_BAT_10		(uint32_t) 30*1000
#define FREQUENCY_NOTIFICATION_LOW_BAT_20		(uint32_t) 120*1000
void batteryChargeNotify(void)
{
	if (batLevelAvg[NB_OF_AVG_VALUES - 1] <= BATTERY_LEVEL_10)
	{
		// doLedRun every 30 seconds
		if ((millis() - previousMillisPixelRun)
				>= FREQUENCY_NOTIFICATION_LOW_BAT_10)
		{
			previousMillisPixelRun = millis();
			// No doPixelRun if we are charging
			if(!isCharging)
			{
				doPixelRun(COLOR_RED, 1000, 1, PIXEL_RUN_CLOCKWISE);
			}
		}
	}
	else if (batLevelAvg[NB_OF_AVG_VALUES - 1] <= BATTERY_LEVEL_20)
	{
		// doLedRun every 120 seconds
		if ((millis() - previousMillisPixelRun)
				>= FREQUENCY_NOTIFICATION_LOW_BAT_20)
		{
			previousMillisPixelRun = millis();
			// No doPixelRun if we are charging
			if(!isCharging)
			{
				doPixelRun(COLOR_RED, 1000, 1, PIXEL_RUN_CLOCKWISE);
			}
		}
	}
}

int batteryChargeDetection(void)
{
	uint8_t pin_value = digitalRead(CHARGING_STATUS_PIN);
	uint8_t last_charging_status = isCharging;
	uint8_t change_detected = 0;

	isCharging = !pin_value;

	if (last_charging_status != isCharging)
	{
		change_detected = 1;
#ifdef DEBUG_SERIAL
		Serial.println("isCharging = " + String(isCharging));
#endif
	}

	return change_detected;
}

void showBatteryLevel(void)
{
	color_t color1 = NO_COLOR;
	color_t color2 = NO_COLOR;
	color_t color3 = NO_COLOR;
	color_t color4 = NO_COLOR;

#ifdef DEBUG_SERIAL
  Serial.println("batLevelAvg[NB_OF_AVG_VALUES - 1] = " + String(batLevelAvg[NB_OF_AVG_VALUES - 1]));
#endif

	if(batLevelAvg[NB_OF_AVG_VALUES - 1] > BATTERY_LEVEL_20)
	{
		color1 = COLOR_GREEN;
	}
	if(batLevelAvg[NB_OF_AVG_VALUES - 1] > BATTERY_LEVEL_40)
	{
		color2 = COLOR_GREEN;
	}
	if(batLevelAvg[NB_OF_AVG_VALUES - 1] > BATTERY_LEVEL_60)
	{
		color3 = COLOR_GREEN;
	}
	if(batLevelAvg[NB_OF_AVG_VALUES - 1] > BATTERY_LEVEL_80)
	{
		color4 = COLOR_GREEN;
	}

	doLedRingFourPixelsBlink(color1, color2, color3, color4, 200, 3);
}

#endif /* BATTERY_H_ */
