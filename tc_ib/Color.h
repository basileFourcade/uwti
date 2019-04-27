// COLOR
#ifndef __COLOR_H__
#define __COLOR_H__

enum color_t
{
	NO_COLOR = 0,
	COLOR_RED = 1,
	COLOR_YELLOW = 2,
	COLOR_GREEN = 3,
	COLOR_INDIGO = 4,
	COLOR_BLUE = 5,
	COLOR_PURPLE = 6,
	COLOR_WHITE = 7,
	COLOR_ORANGE = 8,
	NB_OF_COLOR = 9
};

void colorToRGB(color_t color, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t intensity)
{
	if( (red == NULL) || (green == NULL) || (blue == NULL))
	{
		return;
	}

	switch (color)
	{
		default:
			//no break
		case NO_COLOR:
			*red = 0;
			*green = 0;
			*blue = 0;
			break;

		case COLOR_RED:
			*red = intensity;
			*green = 0;
			*blue = 0;
			break;

		case COLOR_YELLOW:
			*red = intensity;
			*green = intensity;
			*blue = 0;
			break;

		case COLOR_GREEN:
			*red = 0;
			*green = intensity;
			*blue = 0;
			break;

		case COLOR_INDIGO:
			*red = 0;
			*green = intensity;
			*blue = intensity;
			break;

		case COLOR_BLUE:
			*red = 0;
			*green = 0;
			*blue = intensity;
			break;

		case COLOR_PURPLE:
			*red = intensity;
			*green = 0;
			*blue = intensity;
			break;

		case COLOR_WHITE:
			*red = intensity;
			*green = intensity;
			*blue = intensity;
			break;

		case COLOR_ORANGE:
			*red = intensity;
			*green = intensity/2;
			*blue = 0;
			break;

	}
}
#endif //__COLOR_H__
