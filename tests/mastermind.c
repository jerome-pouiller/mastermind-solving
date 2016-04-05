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

