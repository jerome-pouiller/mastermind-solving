/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 15:51:50+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 *
 */
#include <stdio.h>
#include <limits.h>
#include "mastermind.h"

static const int minMaxDepth = 1;
static int initialDepth = 0;
static int isMasterSearch = 0;

void indent(int isMin, int depth) {
    int i;
    int start = 0;

    if (!isMasterSearch && !isMin)
        start = 1;
    if (isMasterSearch == isMin)
        printf("  ");
    for (i = start; i < initialDepth - depth; i++)
        printf("    ");
}

void dbg_start(shot_t history[], colorlist_t *colors, int depth, int masterSearch, void *priv) {
    initialDepth = depth;
    isMasterSearch = masterSearch;
    printf("Given this history:\n");
    prShots(history, 0, 0);
    if (isMasterSearch) {
        printf("Looking for the best master shot in %d depth\n", initialDepth);
    } else {
        printf("Looking for the best player shot in %d depth\n", initialDepth);
    }
}

void dbg_end(shot_t history[], colorlist_t *colors, int depth, shot_t results[], int score, int isMasterSearch, void *priv) {
    printf("Final: Best score: %d\n", score);
    if (isMasterSearch) {
        printf("Final: Number of best scores: %d (real: %d):\n", getNumShots(results, '<', score), getNumRealShots(results, '<', score));
        prShots(results, '<', score);
    } else {
        printf("Final: Number of best scores: %d (real: %d):\n", getNumShots(results, '>', score), getNumRealShots(results, '>', score));
        prShots(results, '>', score);
    }
    printf("Final: All possible shots: %d (real: %d)\n", getNumShots(results, 0, 0), getNumRealShots(results, 0, 0));
    prShots(results, 0, 0);
}

void *dbg_inMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, void *priv, void *dbg_local) {
    indent(1, minMaxDepth);
    prShot(history[getNumShots(history, 0, 0) - 1]);
    if (minMaxDepth)
        printf(" {\n");
    return NULL;
}

void dbg_outMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, int min, void *priv, void *dbg_local) {
    if (minMaxDepth) {
        indent(1, minMaxDepth);
        printf("}");
    }
    if (min != INT_MAX)
        printf(" -> min: %d\n", min);
    else
        printf(" -> impossible\n");
}

void *dbg_inMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, void *priv, void *dbg_local) {
    indent(0, minMaxDepth);
    prShot(history[getNumShots(history, 0, 0) - 1]);
    printf(" {\n");
    return NULL;
}

void dbg_outMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, int max, void *priv, void *dbg_local) {
    indent(0, minMaxDepth);
    printf("}");
    printf(" -> max: %d\n", max);
}

debug_t debugPrint = {
    .priv = NULL,
    .start = dbg_start,
    .end = dbg_end,
    .inMin = dbg_inMin,
    .outMin = dbg_outMin,
    .inMax = dbg_inMax,
    .outMax = dbg_outMax,
};

int main(int argc, char **argv) {
    playerPossibleShots_t results;
    shot_t history[] = {
        S(A, A, B, C, -1, -1),
        S()
    };

    getBestShot(history, minMaxDepth, results.d, &debugPrint);
    return 0;
}

