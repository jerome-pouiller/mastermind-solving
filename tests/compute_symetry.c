/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-04-05 20:13:46+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include "mastermind.h"
#include <stdio.h>

int main(int argc, char **argv) {
    shot_t history[] = {
        S(A, A, A, A, 0, 0),
        S('0', '0', '0', '0', 0, 0),
        S()
    };
    shot_t propositons[] = {
        S(A, A, A, A,   0, 0), // symetry = 1
        S(A, A, A, '0', 0, 0), // symetry = 1
        S(A, A, A, '1', 0, 0), // symetry = 4
        S()
    };
    colorlist_t colors;
    getUsedColors(history, &colors);
    computeSymetries(propositons, &colors);
    prShots(propositons, -1);

    return 0;
}

