/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2015-12-14 19:58:19+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>
#include <string.h>

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

typedef int prop_t[NB_PLACES + 2];
typedef int hint_t[NB_PLACES + 2];

/**
 * Check if a proposition is valid compared to one hint
 */
int checkone(const hint_t hint, const prop_t prop) {
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
int check(hint_t *hints, const prop_t prop) {
    int i;
    for (i = 0; hints[i][0]; i++) {
        if (checkone(hints[i], prop) == -1)
            return -1;
    }
    return 0;
}

/*
 * Generate a list of possibilities.
 *  - possibilities must be big enough (pow(NB_COLORs, NB_PLACES))
 *  - colors is set of already used colors (if num of already used colors is == NB_COLORS, no jocker are used)
 *  - pattern is used internally. It must be filled with zeros
 * Return number of possibilities generated
 */
int generate(int *colors, prop_t *possibilities, prop_t pattern) {
    int jocker = '0';
    int pattern_len, colors_len;
    int total = 0;
    int i;

    for (i = 0; pattern[i]; i++)
        ;
    pattern_len = i;

    for (i = 0; colors[i]; i++) {
        if (colors[i] >= jocker && colors[i] < '0' + NB_COLORS)
            jocker = colors[i] + 1;
    }
    colors_len = i;
    
    // Last iteration of for jocker
    for (i = 0; i < colors_len + 1; i++) {
        if (i < NB_COLORS) {
            if (i == colors_len)
                colors[colors_len] = jocker;
            pattern[pattern_len] = colors[i];
            if (pattern_len < NB_PLACES - 1) {
                total += generate(colors, possibilities + total, pattern);
            } else {
                memcpy(possibilities[total], pattern, sizeof(prop_t));
                total++;
            }
            if (i == colors_len)
                colors[colors_len] = 0;
        }
    }
    pattern[pattern_len] = 0;
    if (!pattern_len)
        possibilities[total][0] = 0;
    return total;
}

int mark(hint_t *hints, prop_t *possibilities, int *colors) {
    int total = 0;
    int i;
    int nb_unknown;

    for (i = 0; colors[i]; i++)
        ;
    nb_unknown = NB_COLORS - i;
    for (i = 0; possibilities[i][0]; i++) {
        if (possibilities[i][NB_PLACES] != -1)
            possibilities[i][NB_PLACES] = check(hints, possibilities[i]);
        if (possibilities[i][NB_PLACES] != -1) {
            char jocker = '0';
            int cnt = 1;
            int j;
            for (j = 0; j < NB_PLACES; j++) {
                if (possibilities[i][j] >= jocker && possibilities[i][j] < '0' + NB_COLORS) {
                    cnt *= nb_unknown - (jocker - '0');
                    jocker = possibilities[i][j] + 1;
                }
            }
            total += cnt;
        }
    }
    return total;
}

void print_prop(prop_t *possibilities) {
    int i, j;

    for (i = 0; possibilities[i][0]; i++) {
        printf("  ");
        for (j = 0; j < NB_PLACES; j++)
            printf("%c", possibilities[i][j]);
        printf("->");
        for (; j < NB_PLACES + 2; j++)
            printf("%d", possibilities[i][j]);
        printf("\n");
    }
}

hint_t history[] = {
    // { B, B, B, B, 0, 0 },
    { A, A, A, A, 0, 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 }
};

int main(int argc, char **argv) {
    prop_t poss[3000];
    int colors[NB_COLORS] = { A, 0 };
    prop_t tmp = { };
    int num = generate(colors, poss, tmp);
    printf("%d\n", num);
    num = mark(history, poss, colors);
    print_prop(poss);
    printf("%d possibilities\n", num);

    return 0;
}

