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
#ifndef __MODE_H__
#define __MODE_H__

/* Modes */
#ifdef CONFIG_ALL_MODES
typedef enum
{
	MODE_SLEEP = 0,
	MODE_1_GAME = 1,
	MODE_2A_GAME = 2,
	MODE_2B_GAME = 3,
	MODE_3_GAME = 4,
	MODE_4_CUSTOM = 5,
	MODE_1_GAME_V2 = 6,
	MODE_2A_GAME_V2 = 7,
	MODE_2B_GAME_V2 = 8,
	MODE_3_GAME_V2 = 9,
	MODE_4_CUSTOM_V2 = 10,
	NB_OF_MODE = 11,
}mode_t;
#elif defined(CONFIG_3_MODES)
typedef enum
{
	MODE_1_GAME = 0,
	MODE_2A_GAME = 1,
	MODE_2B_GAME = 2,
	NB_OF_MODE = 3,
}mode_t;
#endif

#endif // __MODE_H__
