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
	COLOR_PINK = 9,
	NB_OF_COLOR = 10
};

void colorToRGB(color_t color, uint8_t* red, uint8_t* green, uint8_t* blue,
		uint8_t intensity)
{
	if ((red == NULL) || (green == NULL) || (blue == NULL))
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
		*green = intensity / 2;
		*blue = 0;
		break;

	case COLOR_PINK:
		*red = intensity;
		*green = intensity / 2;
		*blue = intensity / 2;
		break;
	}
}
#endif //__COLOR_H__
