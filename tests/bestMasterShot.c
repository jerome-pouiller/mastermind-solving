/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 15:51:50+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>
#include <mastermind.h>

static const int minMaxDepth = 1;

int main(int argc, char **argv) {
    int ret;
    playerPossibleShots_t results;
    shot_t history[] = {
        { A, A, B, C, -1, -1 },
        { -1 }
    };

    ret = getBestShot(history, minMaxDepth, results, NULL)
    printf("Best score : %d\n", ret);
    printf("Number of best scores : %d\n", getNumShots(results, ret), getNumRealShots(results, ret));
    prShots(results, ret);
    return 0;
}

