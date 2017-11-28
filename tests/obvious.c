/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-07-10 09:33:45+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
/* Display remaining possibilities for a few obvious situation */
#include <stdio.h>
#include <limits.h>
#include "mastermind.h"

int main(int argc, char **argv) {
    shot_t histories[6][3] = {
        { S(A, B, C, D, 3, 1), S(0) }, // Impossible
        { S(A, A, A, A, 0, 2), S(0) }, // Impossible
        { S(A, A, A, A, 0, 3), S(0) }, // Impossible
        { S(A, A, A, A, 0, 4), S(0) }, // Impossible
        { S(A, A, A, B, 2, 2), S(0) }, // Trois possibilités
        { S(B, A, D, C, 0, 4), S(A, B, D, C, 2, 2), S(0) } // Une possibilité
    };
    int i, j;

    for (i = 0; i < 6; i++) {
        playerPossibleShots_t results;
        colorlist_t colors;

        getUsedColors(histories[i], &colors);
        getPossiblePlayerShots(&colors, &results);
        computeSymetries(results.d, &colors); // optionnal
        printf("Results for job %d\n", i);
        for (j = 0; results.d[j].d[0]; j++) {
            if (check(histories[i], &results.d[j]) == 0) {
                printf("  ");
                prShot(results.d[j]);
                printf("\n");
            }
        }
    }
    return 0;
}

