/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 16:32:51+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#ifndef MASTERMIND_H
# define MASTERMIND_H

// Number of color in game
#define NB_COLORS        6
// Number of place in game
#define NB_PLACES        4

// Number of place for hints
#define NB_HINTS         2

// Number of possible shots for player (used to store results)
#define NB_POSS_PLAYER   (6 * 6 * 6 * 6) // 6 ** 4

// Number of possible shots for master (used to store results)
// Note: if (NB_PLACES - 1) are correctly placed, it is not possible to have 1 wrong placed
#define NB_POSS_MASTER   (((NB_PLACES + 1) * (NB_PLACES + 2)) / 2 - 1)

typedef struct {
    int d[NB_PLACES + NB_HINT + NB_EXTRA];
} shot_t

typedef struct {
    int d[NB_COLORS + 1];
} colorslist_t;

typedef struct {
    shot_t d[NB_POSS_PLAYER + 1];
} playerPossibleShots_t;

typedef struct {
    shot_t d[NB_POSS_MASTER + 1];
} masterPossibleShots_t;

// Helpers to define histories
#define A 'A'
#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'
#define F 'F'
#define G 'G'

/*
 * Get list of used colors in shot list
 */
int getUsedColors(shot_t history[], colorlist_t *colors);

/*
 * Get list of possible shots for player with given colorlist. Use jocker for
 * not yet used colors. Result size must be at least SIZE_POSS_PLAYER
 */
int getPossiblePlayerShots(colorlist_t *colors, playerPossibleShots_t *results);

/*
 * Get list of possible shots for master. Result size must be at least
 * SIZE_POSS_MASTER
 */
int getPossibleMasterShots(masterPossibleShots_t *results);

/*
 * Get list of possible shots with given history. If last entry of entry do not
 * contains hint (ie. { '0', '0', '1', '2', -1, -1 }), return possible shots
 * from master.
 */
int getPossibleGameShots(shot_t history[], playerPossibleShots_t *results);

/*
 * Get best shots given an history. If last entry of entry do not contains hint
 * (ie. { '0', '0', '1', '2', -1, -1 }), give the best shots for player. Return
 * best score. dbg contains functions that are called during step of algorithm.
 * Use it to print/collect internal information.
 */
int getBestShot(shot_t history[], int minMaxDepth, playerPossibleShots_t *results, debug_t *dbg);

/*
 * From a shot list, return number of entries with a score better or equal to score
 */
int getNumShots(shot_t shots[], int score);

/*
 * From a shot list, return real (taking acount jockers) number of shots with a
 * score better or equal to score.
 */
int getNumRealShots(shot_t shots[], colorlist_t *colors, int score);

/*
 * Filter results to only keep results with a score higher then score. Return
 * number of results.
 */
int filterShots(shot_t in[], shot_t out[], int score);

/*
 * Pretty print result with a score high then score. Return number of printed
 * results.
 */
int prShots(shot_t shots[], int score);

/*
 * Pretty print one shot
 */
int prShot(const shot shot);

#endif /* MASTERMIND_H */

