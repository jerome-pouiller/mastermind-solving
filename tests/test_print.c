/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-04-05 19:39:09+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include "mastermind.h"

#include <stdio.h>

#define I(...)  { .d = { __VA_ARGS__ } };

int main(int argc, char **argv) {
    int ret;

    shot_t solo = S(A, B, C, D,         0, 3);
    shot_t history[] = {
        S( A, B, C, D,         0, 3 ),
        S( '0', '1', '2', '3', 1, 2 ),
        S( A, B, C, D,         2, 1,   3, 4 ),
        S( A, B, C, D,         3, 0, 123, 1 ),
        S( ),
    };
    masterPossibleShots_t master;

    prShot(solo);
    printf("\n");
    prShot(history[0]);
    printf("\n");
    ret = prShots(history, -1);
    printf("Printed %d shots\n", ret);
    ret = prShots(history, 1);
    printf("Printed %d shots\n", ret);
    ret = filterShots(master.d, history, 1);
    printf("Got %d shots\n", ret);
    ret = prShots(master.d, -1);
    printf("Printed %d shots\n", ret);

    return 0;
}

