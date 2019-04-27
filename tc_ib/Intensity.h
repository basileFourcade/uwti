// INTENSITY
#ifndef __INTENSITY_H__
#define __INTENSITY_H__

const uint8_t PHOTO_RESIST = A2;

uint32_t readLightSensor(void)
{
	uint32_t sensor_value = analogRead(PHOTO_RESIST);

#ifdef DEBUG_SERIAL
	Serial.println("Sensor = " + String(sensor_value));
#endif
	return sensor_value;
}

uint8_t getIntensityWithLightSensorValue(uint32_t sensor_value)
{
	/* Observed values from 6 to 970 */
	/* Intensity goes from 0 to 255 */

	/* Let's divide by 8 */
	uint8_t intensity = sensor_value>>3;

	return intensity;
}

/* TODO: VALUE_COLOR_MIN/VALUE_COLOR_MEDIUM/VALUE_COLOR_MAX
 should be in NVM config data */
#define INTENSITY_MIN_DEFAULT		10
#define INTENSITY_MEDIUM_DEFAULT	80
#define INTENSITY_MAX_DEFAULT		150

uint8_t intensity_min		=	INTENSITY_MIN_DEFAULT;
uint8_t intensity_medium	=	INTENSITY_MEDIUM_DEFAULT;
uint8_t intensity_max		=	INTENSITY_MAX_DEFAULT;

#define VALUE_COLOR_MIN			intensity_min
#define VALUE_COLOR_MEDIUM		intensity_medium
#define VALUE_COLOR_MAX			intensity_max

void updateIntensityWithLightSensorValue(uint32_t sensor_value)
{
	/* Observed values from 6 to 970 */
	/* Intensity goes from 0 to 255 */

	intensity_min = sensor_value>>4;
	intensity_medium = sensor_value>>3;
	intensity_max = sensor_value>>2;

#ifdef DEBUG_SERIAL
	Serial.println("intensity_min = " + String(intensity_min));
	Serial.println("intensity_medium = " + String(intensity_medium));
	Serial.println("intensity_max = " + String(intensity_max));
#endif
}

#endif //__INTENSITY_H__
