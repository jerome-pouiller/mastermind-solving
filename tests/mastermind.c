/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-04-05 19:38:46+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include "mastermind.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DEBUG

void prShot(const shot_t shot) {
    int i;

    for (i = 0; i < NB_PLACES; i++)
        printf("%c", shot.d[i]);
    printf(" ");
    for (; i < NB_PLACES + NB_HINTS; i++)
        printf("%d", shot.d[i]);
#ifdef DEBUG
    printf(" sym: %3d", shot.d[IDX_NUM_SYM]);
    printf(" scor: %3d", shot.d[IDX_SCORE]);
#endif
}

int prShots(shot_t shots[], int score) {
    int i;
    int num = 0;

    for (i = 0; shots[i].d[0]; i++) {
        if (score == -1 || shots[i].d[IDX_SCORE] <= score) {
            num++;
            printf("  ");
            prShot(shots[i]);
            printf("\n");
        }
    }
    return num;
}

int getNumShots(shot_t shots[], int score) {
    int i;
    int num = 0;

    for (i = 0; shots[i].d[0]; i++)
        if (score == -1 || shots[i].d[IDX_SCORE] <= score)
            num++;
    return num;
}

int getNumRealShots(shot_t shots[], int score) {
    int i;
    int num = 0;

    for (i = 0; shots[i].d[0]; i++)
        if (score == -1 || shots[i].d[IDX_SCORE] <= score) {
            assert(shots[i].d[IDX_NUM_SYM]); // Uninitialized value
            num += shots[i].d[IDX_NUM_SYM];
        }
    return num;
}

// Given number of unknown colors and last jocker used, compute symetries of a
// shot.
static int _computeSymetries(shot_t *shot, char lst_jocker, int nb_unknown) {
    int i;
    int cnt = 1;
    for (i = 0; i < NB_PLACES; i++) {
        if (shot->d[i] > lst_jocker && shot->d[i] < JOCKER_OFFSET + NB_COLORS) {
            lst_jocker = shot->d[i];
            cnt *= nb_unknown;
            nb_unknown--;
        }
    }
    assert(cnt > 0);
    return cnt;
}

int computeSymetries(shot_t shots[], colorlist_t *colors) {
    int total = 0;
    int i, j;
    int nb_unknown;
    char lst_jocker = JOCKER_OFFSET - 1;

    for (i = 0; colors->d[i]; i++)
        if (colors->d[i] > lst_jocker && colors->d[i] < JOCKER_OFFSET + NB_COLORS)
            lst_jocker = colors->d[i];
    nb_unknown = NB_COLORS - i;
    for (i = 0; shots[i].d[0]; i++) {
        shots[i].d[IDX_NUM_SYM] = _computeSymetries(shots + i, lst_jocker, nb_unknown);
        total += shots[i].d[IDX_NUM_SYM];
    }
    return total;
}

int getUsedColors(shot_t history[], colorlist_t *colors) {
    int i, j, k;
    int nb = 0;
    memset(colors, 0, sizeof(*colors));
    for (i = 0; history[i].d[0]; i++) {
        for (j = 0; j < NB_PLACES; j++) {
            for (k = 0; k < nb && colors->d[k] != history[i].d[j]; k++)
                ;
            if (k == nb)
                colors->d[nb++] = history[i].d[j];
        }
    }
    return nb;
}


int filterShots(shot_t out[], shot_t in[], int score) {
    int i, j = 0;

    for (i = 0; in[i].d[0]; i++)
        if (in[i].d[IDX_SCORE] <= score) {
            memcpy(out + j, in + i, sizeof(shot_t));
            j++;
        }
    out[j].d[0] = 0;
    return j;
}

/**
 * Check if a proposition is valid compared to one hint
 */
static int checkone(const shot_t *hint, const shot_t *prop) {
    int i, j;
    int count;
    int used[NB_PLACES] = { 0 };

    count = 0;
    for (i = 0; i < NB_PLACES; i++)
        if (prop->d[i] == hint->d[i]) {
            used[i] = 3;
            count++;
        }
    if (count != hint->d[IDX_HINT_PLACE])
        return -1;
    count = 0;
    for (i = 0; i < NB_PLACES; i++)
        if (!(used[i] & 1))
            for (j = 0; j < NB_PLACES; j++)
                if (!(used[j] & 2) && prop->d[i] == hint->d[j]) {
                    //used[i] |= 1;
                    used[j] |= 2;
                    count++;
                }
    if (count != hint->d[IDX_HINT_COLOR])
        return -1;

    return 0;
}

/**
 * Check if a proposition is valid compared to multiple hints
 */
static int check(shot_t hints[], const shot_t *prop) {
    int i;
    for (i = 0; hints[i].d[0]; i++)
        if (checkone(hints + i, prop) == -1)
            return -1;
    return 0;
}


/*
 * Generate a list of possibilities.
 *  - possibilities must be big enough (pow(NB_COLORs, NB_PLACES))
 *  - colors is set of already used colors (if num of already used colors is == NB_COLORS, no jocker are used)
 *  - pattern is used internally. It must be filled with zeros
 * Return number of possibilities generated
 */
static int _getPossiblePlayerShots(colorlist_t *colors, shot_t *results, shot_t *pattern) {
    int jocker = JOCKER_OFFSET;
    int pattern_len, colors_len;
    int total = 0;
    int i;

    for (i = 0; pattern->d[i]; i++)
        ;
    pattern_len = i;

    for (i = 0; colors->d[i]; i++) {
        if (colors->d[i] >= jocker && colors->d[i] < JOCKER_OFFSET + NB_COLORS)
            jocker = colors->d[i] + 1;
    }
    colors_len = i;

    for (i = 0; i < colors_len + 1; i++) {
        if (i < NB_COLORS) {
            if (i == colors_len)
                colors->d[colors_len] = jocker;
            pattern->d[pattern_len] = colors->d[i];
            if (pattern_len < NB_PLACES - 1) {
                total += _getPossiblePlayerShots(colors, results + total, pattern);
            } else {
                memcpy(results + total, pattern, sizeof(shot_t));
                total++;
            }
            if (i == colors_len)
                colors->d[colors_len] = 0;
        }
    }
    pattern->d[pattern_len] = 0;
    if (!pattern_len)
        results[total].d[0] = 0;
    return total;
}

int getPossiblePlayerShots(colorlist_t *colors, playerPossibleShots_t *results) {
    shot_t pattern = S();
    return _getPossiblePlayerShots(colors, results->d, &pattern);
}
