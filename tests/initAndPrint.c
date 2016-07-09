/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-04-05 19:39:09+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include "mastermind.h"

#include <stdio.h>

int main(int argc, char **argv) {
    int ret;

    shot_t solo = S(A, B, C, D);
    shot_t history[] = {
        S( A, B, C, D,     0, 3 ),
        S( A, B, '0', '1', 1, 2 ),
        S( A, B, C, D,     2, 1,   3, 4 ),
        S( A, B, C, D,     3, 0, 123, 1 ),
        S( ),
    };
    masterPossibleShots_t master;
    colorlist_t colors;

    prShot(solo);
    printf("\n");
    prShot(history[0]);
    printf("\n");
    ret = prShots(history, 0, 0);
    printf("Printed %d shots\n", ret);
    ret = prShots(history, '>', 1);
    printf("Printed %d shots\n", ret);
    ret = filterShots(master.d, history, '>', 1);
    printf("Got %d shots\n", ret);
    ret = prShots(master.d, 0, 0);
    printf("Printed %d shots\n", ret);
    ret = prShots(master.d, 0, 0);
    printf("Printed %d shots\n", ret);
    ret = getUsedColors(history, &colors);
    printf("%d colors are used\n", ret);

    return 0;
}

