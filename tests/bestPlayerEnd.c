/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 15:51:50+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>
#include "mastermind.h"

static const int minMaxDepth = 5;

int main(int argc, char **argv) {
    int ret;
    playerPossibleShots_t results;
    shot_t history[] = {
        //S(D, C, B, A, 0, 4),
        S(A, B, D, C, 2, 2),
        //S(A, B, C, D, 4, 0),
        S()
    };

    ret = getBestShot(history, minMaxDepth, results.d, NULL);
    printf("Best score: %d\n", ret);
    printf("Number of best scores: %d (real: %d):\n", getNumShots(results.d, '>', ret), getNumRealShots(results.d, '>', ret));
    prShots(results.d, '>', ret);
    printf("All possible shots: %d (real: %d)\n", getNumShots(results.d, 0, 0), getNumRealShots(results.d, 0, 0));
    prShots(results.d, 0, 0);
    return 0;
}

