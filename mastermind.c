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
#define NB_EXTRA_PLACES  4
#define IDX_PLACE_OK     (NB_PLACES + 0)
#define IDX_COLOR_OK     (NB_PLACES + 1)
#define IDX_NUM_SYM_POSS (NB_PLACES + 2)
#define IDX_NUM_POSS     (NB_PLACES + 3)
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

struct debug_parm {
     int initial_depth;
     int debug_depth;
     int details;
};

void pr_prop(const prop_t prop);
int pr_proplist(prop_t props[], int score);
int getmax(hint_t *hints, colorlist_t colors, int depth, struct debug_parm *debug_parm, prop_t poss[NB_POSS_MASTER]);
int getmin(hint_t *hints, colorlist_t colors, int depth, struct debug_parm *debug_parm, prop_t poss[NB_POSS_PLAYER]);

/* Return an array of playing possibilities for player.
 * If colors list does not contains all colors, jockers are used (and number of
 * generated possibilities is lower).
 * Return number of generated possibilities
 */
int generate_player(colorlist_t colors, prop_t possibilities[NB_POSS_PLAYER]);
/* Return an array of playing possibilities for master.
 * player_prop is copied in each result.
 * Return number of generated possibilities.
 */
int generate_master(prop_t player_prop, prop_t possibilities[NB_POSS_MASTER]);

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
int _generate_player(colorlist_t colors, prop_t *results, prop_t pattern) {
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

    for (i = 0; i < colors_len + 1; i++) {
        if (i < NB_COLORS) {
            if (i == colors_len)
                colors[colors_len] = jocker;
            pattern[pattern_len] = colors[i];
            if (pattern_len < NB_PLACES - 1) {
                total += _generate_player(colors, results + total, pattern);
            } else {
                memcpy(results[total], pattern, sizeof(prop_t));
                total++;
            }
            if (i == colors_len)
                colors[colors_len] = 0;
        }
    }
    pattern[pattern_len] = 0;
    if (!pattern_len)
        results[total][0] = 0;
    return total;
}

int generate_player(colorlist_t colors, prop_t results[NB_POSS_PLAYER]) {
    prop_t pattern = { };
    return _generate_player(colors, results, pattern);
}

int generate_master(prop_t pattern, prop_t results[NB_POSS_MASTER]) {
    int i = 0;
    int placed, colors;
    for (placed = 0; placed <= NB_PLACES; placed++) {
        for (colors = 0; colors + placed <= NB_PLACES; colors++) {
            if (! (placed == NB_PLACES - 1 && colors == 1)) {
                memcpy(results[i], pattern, sizeof(int) * NB_PLACES);
                results[i][IDX_PLACE_OK] = placed;
                results[i][IDX_COLOR_OK] = colors;
                //results[i][IDX_NUM_SYM_POSS] = pattern[IDX_NUM_SYM_POSS];
                //results[i][IDX_NUM_POSS] = -1;
                i++;
            }
        }
    }
    results[i][0] = 0;
    return i;
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
        if (possibilities[i][IDX_NUM_POSS] != -1)
            possibilities[i][IDX_NUM_POSS] = check(hints, possibilities[i]);
        if (possibilities[i][IDX_NUM_POSS] != -1 || DEBUG == 1) {
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
            possibilities[i][IDX_NUM_SYM_POSS] = cnt;
            if (possibilities[i][IDX_NUM_POSS] != -1) {
                total += cnt;
            }
        }
    }
    return total;
}

/**
 * Apply minmax algorithm
 */
int getmin(hint_t *hints, colorlist_t colors, int depth, struct debug_parm *debug_parm, prop_t poss[NB_POSS_PLAYER]) {
    int num_poss;
    int i, j, k;
    int min = INT_MAX;

    if (debug_parm && debug_parm->initial_depth == 0)
        debug_parm->initial_depth = depth;

    generate_player(colors, poss);
    num_poss = mark(hints, poss, colors);
    if (num_poss == 0) {
        min = 0;
    } else if (!depth || num_poss == 1) {
        for (i = 0; poss[i][0]; i++)
            if (poss[i][IDX_NUM_POSS] != -1)
                poss[i][IDX_NUM_POSS] = num_poss;
        min = num_poss;
    } else {
        int nb_hints;
        for (i = 0; hints[i][0]; i++)
            ;
        nb_hints = i;
        min = INT_MAX;
        for (i = 0; poss[i][0]; i++) {
            if (poss[i][IDX_NUM_POSS] != -1) {
                int colors_local[NB_COLORS + 1];
                prop_t poss_local[NB_POSS_PLAYER] = { };
                memcpy(hints[nb_hints], poss[i], sizeof(hints[nb_hints]));
                memcpy(colors_local, colors, sizeof(colors_local));
                for (j = 0; poss[i][j]; j++) {
                    for (k = 0; colors_local[k] && poss[i][j] != colors_local[k]; k++)
                        ;
                    if (!colors_local[k])
                        colors_local[k] = poss[i][j];
                }
                poss[i][IDX_NUM_POSS] = getmax(hints, colors_local, depth, debug_parm, poss_local);
                assert(poss[i][IDX_NUM_POSS] > 0);
                if (poss[i][IDX_NUM_POSS] < min)
                    min = poss[i][IDX_NUM_POSS];
            }
        }
    }
    if (debug_parm && debug_parm->debug_depth <= depth) {
        printf("%d ", depth);
        for (i = 0; i < debug_parm->initial_depth - depth; i++)
            printf("    ");
        printf("Got min=%d", min);
        if (num_poss == 1)
            printf(" (solved)");
        if (min == 0)
            printf(" (no-solutions)");
        printf("\n");
        if (debug_parm->details <= depth)
            pr_proplist(poss, min);
    }
    return min;
}

// TODO: support depth == 0 and return result in order to implement "master" mode
int getmax(hint_t *hints, colorlist_t colors, int depth, struct debug_parm *debug_parm, prop_t poss[NB_POSS_MASTER]) {
    int max = -1;
    int nb_hints;
    int i;

    if (debug_parm && debug_parm->initial_depth == 0)
        debug_parm->initial_depth = depth;

    assert(depth > 0);
    for (i = 0; hints[i][0]; i++)
        ;
    nb_hints = i - 1;
    
    generate_master(hints[nb_hints], poss);

    for (i = 0; poss[i][0]; i++) {
        prop_t poss_local[NB_POSS_PLAYER] = { };
        memcpy(hints[nb_hints], poss[i], sizeof(hints[nb_hints]));
        poss[i][IDX_NUM_POSS] = getmin(hints, colors, depth - 1, debug_parm, poss_local);
        if (poss[i][IDX_NUM_POSS] > max)
            max = poss[i][IDX_NUM_POSS];
    }
    hints[nb_hints][0] = 0;
    if (debug_parm && debug_parm->debug_depth <= depth) {
        printf("%d ", depth);
        for (i = 0; i < debug_parm->initial_depth - depth; i++)
            printf("    ");
        printf("  Got max=%d\n", max);
        if (debug_parm->details <= depth)
            pr_proplist(poss, max);
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
    struct debug_parm debug_parm = {
        .debug_depth = 1,
        .details = 2
    };
    prop_t poss[NB_POSS_PLAYER];
    int num = generate_player(colors, poss);
    int ret;
    printf("%d\n", num);
    num = mark(history, poss, colors);
    pr_proplist(poss, -1);
    printf("%d possibilities\n", num);

    ret = getmin(history, colors, 2, &debug_parm, poss);
    printf("Best score : %d\n", ret);
    ret = pr_proplist(poss, ret);
    printf("Num best scores : %d\n", ret);
    return 0;
}

