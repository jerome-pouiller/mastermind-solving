/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2015-12-14 19:58:19+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>

#define NB_PLACES 4
#define NB_COLORS 7
#define IDX_PLACE_OK NB_PLACES
#define IDX_COLOR_OK (NB_PLACES + 1)

#define A 'A'
#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'
#define F 'F'
#define G 'G'
#define JOCK '0'

typedef int prop_t[NB_PLACES];
typedef int hint_t[NB_PLACES + 2];

/**
 * Check if a proposition is valid compared to one hint
 */
int checkone(const int *hint /* [NB_PLACES + 2]*/, const int *prop /* [NB_PLACES] */) {
    int i, j;
    int count;
    int used[NB_PLACES] = { 0 };

    count = 0;
    for (i = 0; i < NB_PLACES; i++) 
        if (prop[i] == hint[i]) {
            used[i] = 3;
            count++;
        }
    if (count != hint[IDX_PLACE_OK])
        return -1;
    count = 0;
    for (i = 0; i < NB_PLACES; i++) 
        if (!(used[i] & 1))
            for (j = 0; j < NB_PLACES; j++) 
                if (!(used[j] & 2) && prop[i] == hint[j]) {
                    //used[i] |= 1;
                    used[j] |= 2;
                    count++;
                }
    if (count != hint[IDX_COLOR_OK])
        return -1;

    return 0;
}

/**
 * Check if a proposition is valid compared to multiple hints
 */
int check(int (*hints)[NB_PLACES + 2], const int *prop) {
    int i;
    for (i = 0; hints[i][0] != -1; i++) {
        if (checkone(hints[i], prop) == -1)
            return -1;
    }
    return 0;
}


int history[][NB_PLACES + 2] = {
    { F, B, C, D, 3, 0 },
    { A, B, C, E, 3, 0 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 },
    { -1 }
};

int main(int argc, char **argv) {
    int prop[NB_PLACES] = { A, B, C, D };
int ret;
    ret = check(history, prop);
    printf("%d\n", ret);

    return 0;
}

