/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2015-12-14 19:58:19+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define NB_COLORS        6
#define NB_PLACES        4
#define NB_EXTRA_PLACES  2
#define IDX_PLACE_OK     NB_PLACES
#define IDX_COLOR_OK     (NB_PLACES + 1)
#define IDX_NUM_SYM_POSS NB_PLACES
#define IDX_NUM_POSS     (NB_PLACES + 1)
#define NB_POSS_PLAYER   (6 * 6 * 6 * 6 + 1) // 6 ** 4 + 1
// Note: if (NB_PLACES - 1) are correctly placed, it is not possible to have 1 wrong placed
#define NB_POSS_MASTER   (((NB_PLACES + 1) * (NB_PLACES + 2)) / 2 - 1 + 1)

typedef int prop_t[NB_PLACES + NB_EXTRA_PLACES];
typedef int hint_t[NB_PLACES + NB_EXTRA_PLACES];
typedef int colorlist_t[NB_COLORS + 1];

#define A 'A'
#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'
#define F 'F'
#define G 'G'
#define JOCK '0'
#define DEBUG 1

void pr_prop(const prop_t prop);
int pr_proplist(prop_t props[], int score);
int getmax(hint_t *hints, colorlist_t colors, int depth);
int getmin(hint_t *hints, colorlist_t colors, int depth, prop_t poss[NB_POSS_PLAYER]);

/* Pretty print a propostion. */
void pr_prop(const prop_t prop) {
    int j;

    printf("  ");
    for (j = 0; j < NB_PLACES; j++)
        printf("%c", prop[j]);
    printf("->");
    for (; j < NB_PLACES + NB_EXTRA_PLACES; j++)
        printf(" %3d", prop[j]);
    printf("\n");
}

/* Pretty print a proposition list.
 * Only print propositon with a particular score (= number of possibilities).
 * Pass -1 asscore to disable.
 * Return number of lines displayed.
 */
int pr_proplist(prop_t props[], int score) {
    int i;
    int num = 0;

    for (i = 0; props[i][0]; i++) {
        if (score == -1 || props[i][IDX_NUM_POSS] == score) {
            num++;
            pr_prop(props[i]);
        }
    }
    return num;
}

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
int generate(colorlist_t colors, prop_t *possibilities, prop_t pattern) {
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

/**
 * Using hints, mark possibilities valid or not. Return number of possibilities
 * taking into account jockers. colors is only used to compute number of
 * possibilities.
 */
int mark(hint_t *hints, prop_t *possibilities, colorlist_t colors) {
    int total = 0;
    int i;
    int nb_unknown;
    char jocker = '0' - 1;

    for (i = 0; colors[i]; i++)
        if (colors[i] > jocker && colors[i] < '0' + NB_COLORS)
            jocker = colors[i];
    nb_unknown = NB_COLORS - i;
    for (i = 0; possibilities[i][0]; i++) {
        if (possibilities[i][NB_PLACES] != -1)
            possibilities[i][NB_PLACES] = check(hints, possibilities[i]);
        if (possibilities[i][NB_PLACES] != -1 || DEBUG == 1) {
            char jocker2 = jocker;
            int nb_unknown2 = nb_unknown;
            int cnt = 1;
            int j;
            for (j = 0; j < NB_PLACES; j++) {
                if (possibilities[i][j] > jocker2 && possibilities[i][j] < '0' + NB_COLORS) {
                    jocker2 = possibilities[i][j];
                    cnt *= nb_unknown2;
                    nb_unknown2--;
                }
            }
            assert(cnt > 0);
            possibilities[i][NB_PLACES + 1] = cnt;
            if (possibilities[i][NB_PLACES] != -1) {
                total += cnt;
            }
        }
    }
    return total;
}

/**
 * Apply minmax algorithm
 */
int getmin(hint_t *hints, colorlist_t colors, int depth, prop_t poss[NB_POSS_PLAYER]) {
    int num_poss;
    int i, j, k;
    prop_t tmp = { };
    int min = INT_MAX;

    generate(colors, poss, tmp);
    num_poss = mark(hints, poss, colors);
    if (num_poss == 0)
        return 0;
    if (!depth || num_poss == 1) {
        for (i = 0; poss[i][0]; i++)
            if (poss[i][NB_PLACES] != -1)
                poss[i][NB_PLACES] = num_poss;
        return num_poss;
    } else {
        int nb_hints;
        for (i = 0; hints[i][0]; i++)
            ;
        nb_hints = i;
        min = INT_MAX;
        for (i = 0; poss[i][0]; i++) {
            if (poss[i][NB_PLACES] != -1) {
                int colors_local[NB_COLORS + 1];
                memcpy(hints[nb_hints], poss[i], sizeof(hints[nb_hints]));
                memcpy(colors_local, colors, sizeof(colors_local));
                for (j = 0; poss[i][j]; j++) {
                    for (k = 0; colors_local[k] && poss[i][j] != colors_local[k]; k++)
                        ;
                    if (!colors_local[k])
                        colors_local[k] = poss[i][j];
                }
                poss[i][NB_PLACES] = getmax(hints, colors_local, depth);
                assert(poss[i][NB_PLACES] > 0);
                if (poss[i][NB_PLACES] < min)
                    min = poss[i][NB_PLACES];
            }
        }
        hints[nb_hints][0] = 0;
        return min;
    }
}

// TODO: support depth == 0 and return result in order to implement "master" mode
int getmax(hint_t *hints, colorlist_t colors, int depth) {
    int max = -1;
    int nb_hints;
    int i;
    int tmp;

    assert(depth > 0);
    for (i = 0; hints[i][0]; i++)
        ;
    nb_hints = i - 1;
    
    for (hints[nb_hints][IDX_PLACE_OK] = 0; hints[nb_hints][IDX_PLACE_OK] <= NB_PLACES; hints[nb_hints][IDX_PLACE_OK]++) {
        for (hints[nb_hints][IDX_COLOR_OK] = 0; hints[nb_hints][IDX_PLACE_OK] + hints[nb_hints][IDX_COLOR_OK] <= NB_PLACES; hints[nb_hints][IDX_COLOR_OK]++) {
                prop_t poss_local[NB_POSS_PLAYER] = { };
                tmp = getmin(hints, colors, depth - 1, poss_local);
                if (tmp > max)
                    max = tmp;
        }
    }
    return max;
}

hint_t history[] = {
    //{ B, B, B, B, 1, 0 },
    //{ A, A, A, A, 2, 0 },
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
    colorlist_t colors = { A, B, C, D, E, F, 0 };
    prop_t poss[3000];
    prop_t tmp = { };
    int num = generate(colors, poss, tmp);
    int ret;
    printf("%d\n", num);
    num = mark(history, poss, colors);
    print_props(poss, -1);
    printf("%d possibilities\n", num);

    ret = getmin(history, colors, 1, poss);
    printf("Best score : %d\n", ret);
    ret = print_props(poss, ret);
    printf("Num best scores : %d\n", ret);
    return 0;
}

