// LED_RING
#ifndef __LED_RING__
#define __LED_RING__

/* Itsy Bitsy proto config */
#define PIN_NEO_PIXEL	5
#define NUMBER_PIXELS	4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

void setLedringRGB(uint8_t value_R, uint8_t value_G, uint8_t value_B)
{
	uint8_t i;

	for (i = 0; i < strip.numPixels(); i++)
	{
		strip.setPixelColor(i, strip.Color(value_R, value_G, value_B));
	}
	strip.show();
}

void setLedringColor(color_t color, uint8_t intensity)
{
	uint8_t local_red = 0;
	uint8_t local_green = 0;
	uint8_t local_blue = 0;

	colorToRGB(color, &local_red, &local_green, &local_blue, intensity);

	setLedringRGB(local_red, local_green, local_blue);
}

void doLedRingBlink(color_t color, uint8_t speed_ms, uint8_t loop)
{
	uint8_t local_red = 0;
	uint8_t local_green = 0;
	uint8_t local_blue = 0;
	uint8_t i; // Avoid compiler warning and overflow

	colorToRGB(color, &local_red, &local_green, &local_blue, VALUE_COLOR_MIN);

	for (i = 1; i <= loop; i++)
	{
		setLedringRGB(local_red, local_green, local_blue);
		delay(speed_ms);
		setLedringRGB(0, 0, 0);
		delay(speed_ms);
	}
}

void setLedringFourPixels(color_t color1, color_t color2, color_t color3, color_t color4, uint8_t intensity)
{
	uint8_t local_red = 0;
	uint8_t local_green = 0;
	uint8_t local_blue = 0;
	uint8_t i = 0;

	colorToRGB(color1, &local_red, &local_green, &local_blue, intensity);
	strip.setPixelColor(i++, strip.Color(local_red, local_green, local_blue));

	colorToRGB(color2, &local_red, &local_green, &local_blue, intensity);
	strip.setPixelColor(i++, strip.Color(local_red, local_green, local_blue));

	colorToRGB(color3, &local_red, &local_green, &local_blue, intensity);
	strip.setPixelColor(i++, strip.Color(local_red, local_green, local_blue));

	colorToRGB(color4, &local_red, &local_green, &local_blue, intensity);
	strip.setPixelColor(i++, strip.Color(local_red, local_green, local_blue));

	strip.show();
}

void setLedringOnePixel(uint8_t idx, color_t color, uint8_t intensity)
{
	uint8_t local_red = 0;
	uint8_t local_green = 0;
	uint8_t local_blue = 0;

	colorToRGB(color, &local_red, &local_green, &local_blue, intensity);
	strip.setPixelColor(idx, strip.Color(local_red, local_green, local_blue));

	strip.show();
}

void doLedRingOnePixelFade(uint8_t idx, color_t color, uint8_t intensity_max, uint8_t speed_ms)
{
	uint8_t local_intensity = intensity_max;

	while(local_intensity > 0)
	{
		setLedringOnePixel(idx, color, local_intensity);
		delay(speed_ms);
		local_intensity -= 2;
	}
}

void doLedRingFourPixelsBlink(color_t color1, color_t color2, color_t color3, color_t color4, uint8_t speed_ms,
		uint8_t loop)
{
	for (uint8_t i = 1; i <= loop; i++)
	{
		setLedringFourPixels(color1, color2, color3, color4, VALUE_COLOR_MEDIUM);
		delay(speed_ms);
		setLedringRGB(0, 0, 0);
#ifdef GET_SENSOR_LIGHT_WHEN_BLINK
		updateIntensityWithLightSensorValue(readLightSensor());
#endif
		delay(speed_ms);
	}
}

void doPixelRun(color_t color, uint16_t speed_ms, uint8_t loop)
{
	uint8_t matrix[NUMBER_PIXELS][NUMBER_PIXELS] = { { 0, 0, 0, 1 }, { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };

	for (uint8_t i = 0; i < loop; i++)
	{
		for (uint8_t i = 0; i < NUMBER_PIXELS; i++)
		{
			setLedringFourPixels((color_t) (matrix[i][0] * color), (color_t) (matrix[i][1] * color),
					(color_t) (matrix[i][2] * color), (color_t) (matrix[i][3] * color), VALUE_COLOR_MEDIUM);
			delay(speed_ms);
		}
	}

	/* Leave the LedRing empty*/
	setLedringFourPixels(NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, VALUE_COLOR_MEDIUM);
}

#endif //__LED_RING__
