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
} mode_t;
#elif defined(CONFIG_3_MODES)
typedef enum
{
	MODE_1_GAME = 0,
	MODE_2A_GAME = 1,
	MODE_2B_GAME = 2,
	NB_OF_MODE = 3,
} mode_t;
#endif

#endif // __MODE_H__
