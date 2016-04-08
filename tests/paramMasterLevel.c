/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 15:51:50+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include "mastermind.h"

#define MAX_LEVEL 100
int masterLevel = 50;

int main(int argc, char **argv) {
    int ret;
    int i;
    int normLevel;
    int closestDiff;
    int closestScore;
    playerPossibleShots_t results;
    shot_t history[] = {
        S(A, A, B, C, -1, -1),
        S( )
    };

    assert(masterLevel <= MAX_LEVEL);
    ret = getBestShot(history, 0, results.d, NULL);
    normLevel = masterLevel * ret / MAX_LEVEL;
    closestDiff = INT_MAX;
    for (i = 0; results.d[i].d[0]; i++) {
        int tmp = abs(normLevel - results.d[i].d[IDX_SCORE]);
        if (tmp < closestDiff) {
            closestDiff = tmp;
            closestScore = results.d[i].d[IDX_SCORE];
        }
    }
    printf("All possible shots: %d (real: %d)\n", getNumShots(results.d, 0, 0), getNumRealShots(results.d, 0, 0));
    prShots(results.d, 0, 0);
    filterShots(results.d, results.d, '>', closestScore);
    filterShots(results.d, results.d, '<', closestScore);
    printf("Most adapted score: %d (diff: %d)\n", closestScore, closestDiff);
    prShots(results.d, 0, 0);
    return 0;
}

