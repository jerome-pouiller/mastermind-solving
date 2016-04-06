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
#define IDX_HINT_PLACE   (NB_PLACES + 0)
#define IDX_HINT_COLOR   (NB_PLACES + 1)

// Number of extra places in shot_t
#define NB_EXTRA         2
// Index number of symetry in this shot (should be 0 if there is no jocker).
//  0 means uninitialized
#define IDX_NUM_SYM      (NB_PLACES + NB_HINTS + 0)
// Index of score (=number of possibilityes for player/master after this shot).
// ?? means unitialized
// ?? means shot is possible, but we don't known the score
// -1 means this shot is not possible
//  0 means we garantee this shot to be correct
//  1 means we garantee this shot or next one will be correct
#define IDX_SCORE        (NB_PLACES + NB_HINTS + 1)

// Number of possible shots for player (used to store results)
#define NB_POSS_PLAYER   (6 * 6 * 6 * 6) // 6 ** 4

// Number of possible shots for master (used to store results)
// Note: if (NB_PLACES - 1) are correctly placed, it is not possible to have 1 wrong placed
#define NB_POSS_MASTER   (((NB_PLACES + 1) * (NB_PLACES + 2)) / 2 - 1)


typedef struct {
    int d[NB_PLACES + NB_HINTS + NB_EXTRA];
} shot_t;

typedef struct {
    int d[NB_COLORS + 1];
} colorlist_t;

typedef struct {
    shot_t d[NB_POSS_PLAYER + 1];
} playerPossibleShots_t;

typedef struct {
    shot_t d[NB_POSS_MASTER + 1];
} masterPossibleShots_t;

typedef struct {
} debug_t;

// Helper to initialise shot_t, Exemple:
// shot_t s = S(A, B, C, D, 0, 1);
#define S(...)  { .d = { __VA_ARGS__ } }

// Helpers to define histories
#define COLOR_OFFSET 'A'
#define JOCKER_OFFSET '0'
#define A (COLOR_OFFSET + 0)
#define B (COLOR_OFFSET + 1)
#define C (COLOR_OFFSET + 2)
#define D (COLOR_OFFSET + 3)
#define E (COLOR_OFFSET + 4)
#define F (COLOR_OFFSET + 5)
#define G (COLOR_OFFSET + 6)

/*
 * Get list of used colors in shot list
 */
int getUsedColors(shot_t history[], colorlist_t *colors);

/*
 * Get list of possible shots for player with given colorlist. Use jocker for
 * not yet used colors.
 */
int getPossiblePlayerShots(colorlist_t *colors, playerPossibleShots_t *results);

/*
 * Get list of possible shots for master. You can provide an initial shot that
 * will copied in each result entry.
 * Pass NULL as result if only want to know number of possible shots.
 */
int getPossibleMasterShots(shot_t *currentShot, masterPossibleShots_t *results);

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
 * From a shot list, return number of entries (without taking account jockers)
 * with a score better or equal to score
 */
int getNumShots(shot_t shots[], int score);

/*
 * From a shot list, return real (taking account jockers) number of shots with a
 * score better or equal to score. Make sure symtery information is available in
 * shots (by calling computeSymetries() before, for example).
 */
int getNumRealShots(shot_t shots[], int score);

/*
 * Given already used colors in history, complete all symetries information of
 * a shot list. colors have to contains all used colors (you can use
 * getUsedColors() for that).
 */
int computeSymetries(shot_t shots[], colorlist_t *colors);

/*
 * Filter results to only keep results with a score higher then score. Return
 * number of results.
 */
int filterShots(shot_t out[], shot_t in[], int score);

/*
 * Pretty print result with a score high then score. Return number of printed
 * results.
 */
int prShots(shot_t shots[], int score);

/*
 * Pretty print one shot
 */
void prShot(const shot_t shot);

#endif /* MASTERMIND_H */

