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

int computeSymteries(shot_t shots[], colorlist_t *colors) {
    int total = 0;
    int i, j;
    int nb_unknown_orig;
    char lst_jocker_orig = JOCKER_OFFSET - 1;

    for (i = 0; colors->d[i]; i++)
        if (colors->d[i] > lst_jocker_orig && colors->d[i] < JOCKER_OFFSET + NB_COLORS)
            lst_jocker_orig = colors->d[i];
    nb_unknown_orig = NB_COLORS - i;
    for (i = 0; shots[i].d[0]; i++) {
        char lst_jocker = lst_jocker_orig;
        int nb_unknown = nb_unknown_orig;
        int cnt = 1;
        for (j = 0; j < NB_PLACES; j++) {
            if (shots[i].d[j] > lst_jocker && shots[i].d[j] < JOCKER_OFFSET + NB_COLORS) {
                lst_jocker = shots[i].d[j];
                cnt *= nb_unknown;
                nb_unknown--;
            }
        }
        assert(cnt > 0);
        shots[i].d[IDX_NUM_SYM] = cnt;
        total += cnt;
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

