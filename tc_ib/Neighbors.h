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
// NEIGHBORS
#ifndef __NEIGHBORS_H__
#define __NEIGHBORS_H__

const uint8_t DETECTFACE1 = A0;
const uint8_t DETECTFACE2 = 11;
const uint8_t DETECTFACE3 = A3;
const uint8_t DETECTFACE4 = A1;

#define NB_FACES			4
uint8_t faces[NB_FACES];
uint8_t detect_faces[NB_FACES] =
{ DETECTFACE1, DETECTFACE2, DETECTFACE3, DETECTFACE4 };

void initDetectNeighbors(void)
{
	// Reset variables
	memset(faces, 0x00, sizeof(faces));

	pinMode(DETECTFACE1, INPUT_PULLUP);
	pinMode(DETECTFACE2, INPUT_PULLUP);
	pinMode(DETECTFACE3, INPUT_PULLUP);
	pinMode(DETECTFACE4, INPUT_PULLUP);
}

/* Use to detect specific transition */
static uint8_t last_nb_of_neighbors = 0;

uint8_t detectNeighbors(boolean *isChangeDetected,
		boolean *isFirstNeighborsDetected, boolean *isLastNeighborsLost,
		boolean *noNeighborsUntilNow)
{
	uint8_t nb_of_neighbors = 0;
	*isChangeDetected = false;
	*isFirstNeighborsDetected = false;
	*isLastNeighborsLost = false;

	for (uint8_t i = 0; i < NB_FACES; i++)
	{
		if (digitalRead(detect_faces[i]) == LOW)
		{
			if (faces[i] == 0)
			{
				*isChangeDetected = true;
			}
			faces[i] = 1;
			nb_of_neighbors++;
		}
		else
		{
			if (faces[i] == 1)
			{
				*isChangeDetected = true;
			}
			faces[i] = 0;
		}
	}

	/* Monitor switch from 0 to 1 neighbors */
	if ((last_nb_of_neighbors == 0) && (nb_of_neighbors == 1))
	{
		*isFirstNeighborsDetected = true;
	}
	/* Monitor switch from 1 to 0 neighbors */
	if ((last_nb_of_neighbors == 1) && (nb_of_neighbors == 0))
	{
		*isLastNeighborsLost = true;
	}
	/* Monitor the FIRST switch from 0 to 1 neighbors */
	if (nb_of_neighbors && noNeighborsUntilNow)
	{
		*noNeighborsUntilNow = false;
	}

	/* Store it for next time*/
	last_nb_of_neighbors = nb_of_neighbors;

	return nb_of_neighbors;
}

#endif //__NEIGHBORS_H__
