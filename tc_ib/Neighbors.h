// NEIGHBORS
#ifndef __NEIGHBORS_H__
#define __NEIGHBORS_H__

const uint8_t DETECTFACE1 = A4;
const uint8_t DETECTFACE2 = A1;
const uint8_t DETECTFACE3 = A0;
const uint8_t DETECTFACE4 = A3;

#define NB_FACES			4
uint8_t faces[NB_FACES];
uint8_t detect_faces[NB_FACES] = { DETECTFACE1, DETECTFACE2, DETECTFACE3, DETECTFACE4 };


void initDetectNeighbors(void)
{
	// Reset variables
	memset(faces, 0x00, sizeof(faces));

	pinMode(DETECTFACE1, INPUT_PULLUP);
	pinMode(DETECTFACE2, INPUT_PULLUP);
	pinMode(DETECTFACE3, INPUT_PULLUP);
	pinMode(DETECTFACE4, INPUT_PULLUP);
}

uint8_t detectNeighbors(boolean * isChangeDetected)
{
	uint8_t nb_of_neighbors = 0;
	*isChangeDetected = false;

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

	return nb_of_neighbors;
}

#endif //__NEIGHBORS_H__
